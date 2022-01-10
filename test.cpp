#include<bits/stdc++.h>
using namespace std;
int main()
{
    char buffer[100] = {0};
    FILE* newFile;
    newFile = fopen("nums.txt", "r+");
    while(!feof(newFile))
        fread(buffer, sizeof(buffer), 1, newFile);
    string s(buffer);
    cout << s << endl;
    return 0;
}