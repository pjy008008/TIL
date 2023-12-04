#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
using namespace std;
void insertionSort(vector<int> &arr)
{
    int n = arr.size();
    for (int i = 1; i < n; i++)
    {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}
void bubbleSort(vector<int> &arr)
{
    int n = arr.size();
    bool swapped;
    for (int i = 0; i < n - 1; i++)
    {
        swapped = false;
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (swapped == false)
            break;
    }
}
void selectionSort(vector<int> &arr)
{
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
    {
        int minIndex = i;
        for (int j = i + 1; j < n; j++)
        {
            if (arr[j] < arr[minIndex])
            {
                minIndex = j;
            }
        }
        if (minIndex != i)
        {
            std::swap(arr[i], arr[minIndex]);
        }    
    }
}
int main()
{
    clock_t start, finish;
    double duration;

    vector<int> numbers;
    ifstream inputFile("random_numbers.txt"); // 분석에 사용할 배열 입력

    if (inputFile.is_open())
    {
        int number;
        while (inputFile >> number)
        {
            numbers.push_back(number);
        }
        inputFile.close();

        start = clock();
        selectionSort(numbers); // 분석할 알고리즘 입력
        finish = clock();

        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << duration << "초" << endl;
    }
    else
    {
        cerr << "파일을 열 수 없습니다." << std::endl;
    }

    return 0;
}