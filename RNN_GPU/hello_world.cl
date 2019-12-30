__kernel void HelloWorld(__global char* output) 
{
    output[0] = 'H';
    output[1] = 'e';
    output[2] = 'l';
    output[3] = 'l';
    output[4] = 'o';
    output[5] = ' ';
    output[6] = 'W';
    output[7] = 'o';
    output[8] = 'r';
    output[9] = 'l';
    output[10] = 'd';
    output[11] = '!';
    output[12] = '\n';
}