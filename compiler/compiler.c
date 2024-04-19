#define MAX 100
#include <stdio.h>
#include <string.h>

int action_tbl[12][6] = {
    {5, 0, 0, 4, 0, 0},
    {0, 6, 0, 0, 0, 999},
    {0, -2, 7, 0, -2, -2},
    {0, -4, -4, 0, -4, -4},
    {5, 0, 0, 4, 0, 0},
    {0, -6, -6, 0, -6, -6},
    {5, 0, 0, 4, 0, 0},
    {5, 0, 0, 4, 0, 0},
    {0, 6, 0, 0, 11, 0},
    {0, -1, 7, 0, -1, -1},
    {0, -3, -3, 0, -3, -3},
    {0, -5, -5, 0, -5, -5}};

int goto_tbl[12][4] = {
    {0, 1, 2, 3},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 8, 2, 3},
    {0, 0, 0, 0},
    {0, 0, 9, 3},
    {0, 0, 0, 10},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}};

char lhs[] = {' ', 'E', 'E', 'T', 'T', 'F', 'F'}; // dummy in 0 index
int rhs_len[] = {0, 3, 1, 3, 1, 3, 1};            // rhs length: 0 for dummy rule
char token[] = {'d', '+', '*', '(', ')', '$'};
char NT[] = {' ', 'E', 'T', 'F'}; // non-terminals: dummy in 0 index
int stack[MAX], sp;

void push(int item)
{
    if (sp < MAX)
    {
        stack[sp++] = item;
    }
    else
    {
        printf("Stack Overflow!\n");
    }
}
int pop()
{
    if (sp > 0)
    {
        return stack[--sp];
    }
    else
    {
        printf("Stack Underflow!\n");
        return -1;
    }
}

int get_token(char ch)
{
    for (int i = 0; i < 6; i++)
    {
        if (token[i] == ch)
        {
            return i;
        }
    }
    return -1;
}

void print_stack()
{
    for (int i = 0; i < sp; i++)
    {
        if (stack[i] >= 40 && stack[i] <= 43)
        {
            printf("%c", stack[i]);
        }
        else if (stack[i] >= 65 && stack[i] <= 122)
        {
            printf("%c", stack[i]);
        }
        else
        {
            printf("%d", stack[i]);
        }
    }
    printf("\n");
}

void LR_Parser(char *str)
{
    int count = 1;
    int i = 0;
    push(0);
    printf("(%d) initial\n", count);
    printf("stack=>");
    print_stack();
    printf("input=>%s\n", str + i);

    while (i <= strlen(str))
    {
        int s_top = stack[sp - 1];
        int token_idx = get_token(str[i]);
        int action = action_tbl[s_top][token_idx];
        count++;
        printf("(%d) ", count);
        if (action == 999)
        {
            printf("accept\n");
            break;
        }
        else if (token_idx == -1)
        {
            printf("invalid token (%c) error\n", str[i]);
            break;
        }
        else if (action > 0)
        {
            printf("Shift %d\n", action);
            push(str[i]);
            push(action);
            printf("stack=>");
            print_stack();
            i++;
        }
        else if (action < 0)
        {
            action *= -1;
            int non_terminal_idx = action;
            printf("reduce %d\n", action);
            int reduce_len = rhs_len[non_terminal_idx];
            for (int j = 0; j < reduce_len * 2; j++)
            {
                pop();
            }

            int k = 0;
            for (; k < 4; k++)
            {
                if (NT[k] == lhs[action])
                {
                    break;
                }
            }
            int goto_state = goto_tbl[stack[sp - 1]][k];
            push(lhs[action]);
            push(goto_state);
            printf("stack=>");
            print_stack();
        }
        else
        {
            printf("error\n");
            break;
        }

        printf("input=>%s\n", str + i);
    }
}

int main()
{
    char input[MAX];
    while (1)
    {
        printf("\nInput: ");
        scanf("%s", input);
        if (input[0] == '$')
            break;
        LR_Parser(input);
        sp = 0;
    }
    return 0;
}