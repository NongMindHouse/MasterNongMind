#include <stdio.h>
void PrintArray(int *arr, int n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void GenerateMastermindCodes(int *arr, int size, int l, int r, int code_length)
{
    if (l == code_length)
    {
        PrintArray(arr, code_length);
        return;
    }

    for (int i = 0; i < size; i++)
    {
        arr[l] = i;
        GenerateMastermindCodes(arr, size, l + 1, r, code_length);
    }
}

int main()
{
    int code_length = 4;  
    int num_colors = 6; 
    int code[code_length]; 
    GenerateMastermindCodes(code, num_colors, 0, code_length - 1, code_length);
    return 0;
}
