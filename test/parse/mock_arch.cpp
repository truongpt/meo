#include <unordered_map>
#include <iostream>
#include <vector>
#include <string.h>

#include "error_code.h"
#include "gen_internal.h"

using namespace std;
static char* mock_load(int32_t value, FILE* out_file);
static char* mock_out(char* r, FILE* out_file);
static char* mock_add(char* r1, char* r2, FILE* out_file);
static char* mock_sub(char* r1, char* r2, FILE* out_file);
static char* mock_mul(char* r1, char* r2, FILE* out_file);
static char* mock_div(char* r1, char* r2, FILE* out_file);

// register
vector<char*> reg = {"r0","r1","r2","r3","r4"};

int cur = 0;
int32_t last_value = 0;
unordered_map<char*, int32_t> mem;

int32_t GenLoadX86_64(GenFuncTable *func)
{
    func->f_load = &mock_load;
    func->f_out  = &mock_out;
    func->f_add  = &mock_add;
    func->f_sub  = &mock_sub;
    func->f_mul  = &mock_mul;
    func->f_div  = &mock_div;
    for (auto r : reg) {
        mem[r] = 0;
    }
    cur = 0;
    return Success;
}

int32_t MockGetLatest()
{
    return last_value;
}

char* reg_alloc() {
    if (cur >= reg.size()) {
        cout << "Not have available register" << endl;
        return NULL;
    }
    return reg[cur++];
}

void reg_free(char* r) {
    if (mem.find(r) == mem.end()) {
        cout << "Free reg is not correct" << endl;
        exit(1);
    }
    reg[--cur] = r;
}

static char* mock_load(int32_t value, FILE* out_file)
{
    char* r = reg_alloc();
    if (NULL == r) {
        return NULL;
    }

    mem[r] = value;
    return r;
}

static char* mock_out(char* r, FILE* out_file)
{
    last_value = mem[r];
    reg_free(r);
    return r;
}
static char* mock_add(char* r1, char* r2, FILE* out_file)
{
    mem[r1] += mem[r2];
    mem[r2] = 0;
    reg_free(r2);

    return r1;
}

static char* mock_sub(char* r1, char* r2, FILE* out_file)
{
    mem[r1] -= mem[r2];
    mem[r2] = 0;
    reg_free(r2);

    return r1;
}

static char* mock_mul(char* r1, char* r2, FILE* out_file)
{
    mem[r1] *= mem[r2];
    mem[r2] = 0;
    reg_free(r2);

    return r1;
}

static char* mock_div(char* r1, char* r2, FILE* out_file)
{
    mem[r1] /= mem[r2];
    mem[r2] = 0;
    reg_free(r2);

    return r1;
}
