#include <vector>
using namespace std;

class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        int size = nums.size();
        vector<int> result;
        int sum;
        for(int i=0;i<size;i++){
            for(int j=i+1;j<size;j++){
                sum=nums[i]+nums[j];
                if(sum==target){
                    result.push_back(i);
                    result.push_back(j);
                    break;
                }
            }
        }
        return result;
    }
};