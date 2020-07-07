#pragma once

#include <vector>
#include <queue>
#include <tuple>

#include "Traits.h"


namespace ctl::gph
{
	struct isDirected {};
	struct isUndirected {};
	struct isBoth {};

	using Vertex = size_t;

	struct Weighted
	{
		struct Edge
		{
			Vertex source;
			Vertex dest;
			double weight;
		};
		struct Link
		{
			Vertex dest;
			double weight;
		};

	protected:
		void _push_(const Edge& e) { m_adjList[e.source].push_back({ e.dest, e.weight }); }
		void _pushRev_(const Edge& e) { m_adjList[e.dest].push_back({ e.source, e.weight }); }

		std::vector<std::vector<Link>> m_adjList;
	};

	struct NoWeight
	{
		struct Edge
		{
			Vertex source;
			Vertex dest;
		};
		struct Link
		{
			Vertex dest;
		};

	protected:
		void _push_(const Edge& e) { m_adjList[e.source].push_back({ e.dest }); }
		void _pushRev_(const Edge& e) { m_adjList[e.dest].push_back({ e.source }); }

		std::vector<std::vector<Link>> m_adjList;
	};


	template<typename Weight>
	class Direct : protected Weight
	{
	public:
		using direction_t = isDirected;
		using edge_t = typename Weight::Edge;
		using link_t = typename Weight::Link;

		Direct() = default;

		template<typename Iter>
		Direct(Iter begin, Iter end, size_t size)
		{
			static_assert(std::is_same_v<std::iterator_traits<Iter>::value_type, edge_t>, "Must be of type Edge");

			this->m_adjList.resize(size);
			for (; begin != end; ++begin)
				this->_push_(*begin);
		}

		auto& push(const edge_t& e)
		{
			if (this->m_adjList.size() < e.source + 1)
				this->m_adjList.resize(e.source + 1);

			this->_push_(e);

			return *this;
		}

	};

	template<typename Weight>
	class Undirect : protected Weight
	{
	public:
		using direction_t = isUndirected;
		using edge_t = typename Weight::Edge;
		using link_t = typename Weight::Link;

		Undirect() = default;

		template<typename Iter>
		Undirect(Iter begin, Iter end, size_t size)
		{
			static_assert(std::is_same_v<std::iterator_traits<Iter>::value_type, edge_t>, "Must be of type Edge");

			this->m_adjList.resize(size);
			for (; begin != end; ++begin)
				this->_push_(*begin),
				this->_pushRev_(*begin);
		}

		auto& push(const edge_t& e)
		{
			const auto max = std::max(e.source + 1, e.dest + 1);
			if (this->m_adjList.size() < max)
				this->m_adjList.resize(max);

			this->_push_(e);
			this->_pushRev_(e);

			return *this;
		}

	};


	template<typename Direction, template<typename> class... Func>
	class Graph : public Direction, public Func<Graph<Direction, Func...>>...
	{
	public:
		using direction_t = typename Direction::direction_t;
		using edge_t = typename Direction::edge_t;
		using link_t = typename Direction::link_t;

		using Direction::Direction;

		auto begin() const
		{
			return this->m_adjList.begin();
		}
		auto end() const
		{
			return this->m_adjList.end();
		}

		const auto& linesFrom(size_t n) const
		{
			return this->m_adjList.at(n);
		}

		size_t size() const noexcept
		{
			return this->m_adjList.size();
		}

		void extendTo(size_t size)
		{
			this->m_adjList.resize(size);
		}
	};


	template<typename ImplGraph>
	class Connected : public crtp<ImplGraph, Connected>
	{
		//Mark all unchecked nodes
		std::vector<bool>& _goThrough_(std::vector<bool>& check, Vertex nextNode) const
		{
			check[nextNode] = true;
			//Find node that's not been activated and recurse to it. If none found go back in stack.
			for (auto& i : this->_().linesFrom(nextNode))
				if (!check[i.dest])
					_goThrough_(check, i.dest);
			return check;
		}

	public:
		bool connected(Vertex start = 0) const
		{
			std::vector<bool> table(this->_().size(), false);
			for (const auto& i : _goThrough_(table, start))
				if (!i)
					return false;
			return true;
		}

		bool connected(Vertex start, Vertex with, std::vector<bool>&& checkList = std::vector<bool>(this->_().size(), false))
		{
			return _goThrough_(checkList, start)[with];
		}
	};


	template<typename ImplGraph>
	class Dijkstra : public crtp<ImplGraph, Dijkstra>
	{
	public:
		auto dijkstra(Vertex start) const
		{
			using link_t = typename ImplGraph::link_t;

			std::priority_queue<link_t, std::vector<link_t>, bool (*)(const link_t&, const link_t&)> pq
			([](const link_t& l, const link_t& r) { return l.weight > r.weight; });

			std::vector<double> dist(this->_().size(), std::numeric_limits<double>::max());
			std::vector<bool> visited(this->_().size(), false);

			pq.push({ start, 0. });
			dist[start] = 0.;

			while (!pq.empty())
			{
				const auto current = pq.top().dest;
				pq.pop();
				visited[current] = true;

				//Check if vertex is marked and new calculated distance is lower than the currently saved one.
				for (const auto& x : this->_().linesFrom(current))
					if (!visited[x.dest] && dist[x.dest] > dist[current] + x.weight)
					{
						dist[x.dest] = dist[current] + x.weight;
						pq.push({ x.dest, dist[x.dest] });
					}
			}

			return dist;
		}

		//auto dijkstraWPath(Vertex start) const
		//{
		//	std::priority_queue<mth::Line, std::vector<mth::Line>, bool (*)(const mth::Line&, const mth::Line&)> pq
		//	([](const mth::Line& l, const mth::Line& r) { return l.weight > r.weight; });

		//	std::vector<double> dist(this->_().size(), std::numeric_limits<double>::max());
		//	std::vector<bool> visited(this->_().size(), false);

		//	std::vector<size_t> path(this->_().size());

		//	pq.emplace(0., start);
		//	dist[start] = 0.;
		//	//0 vertex points to nothing
		//	path.front() = -1;

		//	while (!pq.empty())
		//	{
		//		const auto current = pq.top().dest;
		//		pq.pop();
		//		visited[current] = true;

		//		for (const auto& x : this->_()[current])
		//			if (!visited[x.dest] && dist[x.dest] > dist[current] + x.weight)
		//			{
		//				dist[x.dest] = dist[current] + x.weight;
		//				pq.emplace(dist[x.dest], x.dest);

		//				path[x.dest] = current;
		//			}
		//	}

		//	return make_pair(dist, path);
		//}
	};


	template<typename ImplGraph>
	class Transpose : public crtp<ImplGraph, Transpose>
	{
	public:
		static_assert(!std::is_same_v<typename ImplGraph::direction_t, isUndirected>, "Graph must be directed to use this.");

		//Reserse directions of edges
		ImplGraph transpose() const
		{
			ImplGraph out;
			out.extendTo(this->_().size());

			for (size_t i = 0; i < this->_().size(); ++i)
				for (const auto& iter : this->_().linesFrom(i))
					out.push({ iter.dest, i, iter.weight });

			return out;
		}
	};


	//Graph must have the connected function
	template<typename ImplGraph>
	class Eulerian : public crtp<ImplGraph, Eulerian>
	{
	public:
		static_assert(!std::is_same_v<typename ImplGraph::direction_t, isDirected>, "Graph must be undirected to use this.");

		enum { NONE, PATH, CIRCLE };

		Eulerian eulerian() const
		{
			if (!this->_().connected())
				return NONE;

			uint8_t odds = 0;
			for (const auto& i : this->_())
				if (i.size() & 1)
				{
					++odds;
					if (odds > 2) //no point continuing
						return NONE;
				}

			switch (odds)
			{
			case 0:  return CIRCLE;
			case 2:  return PATH;
			default: return NONE;
			}
		}
	};


	template<typename ImplGraph>
	class MST : public crtp<ImplGraph, MST>
	{
	public:
		static_assert(!std::is_same_v<typename ImplGraph::direction_t, isDirected>, "Graph must be undirected to use this.");

		auto minimumSpanningTree() const
		{
			using link_t = typename ImplGraph::link_t;

			std::priority_queue<link_t, std::vector<link_t>, bool (*)(const link_t&, const link_t&)> pq
			([](const link_t& l, const link_t& r) { return l.weight > r.weight; });

			std::vector<double> key(this->_().size(), std::numeric_limits<double>::max());
			std::vector<bool> inMST(this->_().size(), false);

			//Path for new graph
			std::vector<link_t> parent(this->_().size());

			pq.push({ 0, 0. });
			key[0] = 0.;

			while (!pq.empty())
			{
				const auto current = pq.top().dest;
				pq.pop();

				inMST[current] = true;

				for (const auto& i : this->_().linesFrom(current))
					if (!inMST[i.dest] && key[i.dest] > i.weight)
					{
						key[i.dest] = i.weight;
						pq.push({ i.dest, i.weight });

						parent[i.dest] = { current, i.weight };
					}
			}

			ImplGraph mst;
			mst.extendTo(this->_().size());
			for (size_t i = 1; i < parent.size(); ++i)
				mst.push({ i, parent[i].dest, parent[i].weight });

			return mst;
		}
	};

}