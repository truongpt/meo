#include <unordered_map>
#include <iostream>
#include <vector>
#include <string.h>

#include "error_code.h"
#include "gen_internal.h"

using namespace std;
static char* mock_load(int32_t value, FILE* out_file);
static char* mock_out(char* r, FILE* out_file);
static char* mock_print(char* r, FILE* out_file);

static char* mock_add(char* r1, char* r2, FILE* out_file);
static char* mock_sub(char* r1, char* r2, FILE* out_file);
static char* mock_mul(char* r1, char* r2, FILE* out_file);
static char* mock_div(char* r1, char* r2, FILE* out_file);

static char* mock_lt(char* r1, char* r2, FILE* out_file);
static char* mock_le(char* r1, char* r2, FILE* out_file);
static char* mock_gt(char* r1, char* r2, FILE* out_file);
static char* mock_ge(char* r1, char* r2, FILE* out_file);
static char* mock_eq(char* r1, char* r2, FILE* out_file);
static char* mock_ne(char* r1, char* r2, FILE* out_file);

static char* mock_lt_j(char* r1, char* r2, char* l, FILE* out_file);
static char* mock_le_j(char* r1, char* r2, char* l, FILE* out_file);
static char* mock_gt_j(char* r1, char* r2, char* l, FILE* out_file);
static char* mock_ge_j(char* r1, char* r2, char* l, FILE* out_file);
static char* mock_eq_j(char* r1, char* r2, char* l, FILE* out_file);
static char* mock_ne_j(char* r1, char* r2, char* l, FILE* out_file);
static char* mock_jump(char* l, FILE* out_file);
static char* mock_zero_j(char* r, char* l, FILE* out_file);
static char* mock_label(char* l, FILE* out_file);

static char* mock_var(char* var, FILE* out_file);
static char* mock_store(char* r, char* var, FILE* out_file);
static char* mock_load_var(char* var, FILE* out_file);

// register
vector<char*> reg = {"r0","r1","r2","r3","r4"};
unordered_map<string, int32_t> g_variable;
unordered_map<char*, int32_t> mem;

int cur = 0;
vector<int> print_out = {};
int p_index = 0;

int32_t GenLoadX86_64(GenFuncTable *func)
{
    func->f_load  = &mock_load;
    func->f_out   = &mock_out;
    func->f_var   = &mock_var;
    func->f_print = &mock_print;
    func->f_add   = &mock_add;
    func->f_sub   = &mock_sub;
    func->f_mul   = &mock_mul;
    func->f_div   = &mock_div;
    func->f_lt    = &mock_lt;
    func->f_le    = &mock_le;
    func->f_gt    = &mock_gt;
    func->f_ge    = &mock_ge;
    func->f_eq    = &mock_eq;
    func->f_ne    = &mock_ne;
    func->f_lt_j  = &mock_lt_j;
    func->f_le_j  = &mock_le_j;
    func->f_gt_j  = &mock_gt_j;
    func->f_ge_j  = &mock_ge_j;
    func->f_eq_j  = &mock_eq_j;
    func->f_ne_j  = &mock_ne_j;
    func->f_jump  = &mock_jump;
    func->f_zero_j  = &mock_zero_j;
    func->f_label = &mock_label;

    func->f_store    = &mock_store;
    func->f_load_var = &mock_load_var;

    for (auto r : reg) {
        mem[r] = 0;
    }
    cur = 0;
    p_index = 0;
    g_variable.clear();
    print_out.clear();
    
    return Success;
}

int32_t MockGetPrintValue()
{
    if (p_index > print_out.size()) {
        cout << "Not having print out data" << endl;
        return -1;
    };
    return print_out[p_index++];
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
    fprintf(out_file,"[LOAD]:\t %s = %d\n", r, value);
    return r;
}

static char* mock_out(char* r, FILE* out_file)
{
    reg_free(r);
    return r;
}

static char* mock_print(char* r, FILE* out_file)
{
    print_out.push_back(mem[r]);
    reg_free(r);
    fprintf(out_file,"[PRIN]:\t %s\n",r);
    return r;
}

static char* mock_add(char* r1, char* r2, FILE* out_file)
{
    mem[r1] += mem[r2];
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[ADD ]:\t %s = %s+%s\n",r1,r1,r2);
    return r1;
}

static char* mock_sub(char* r1, char* r2, FILE* out_file)
{
    mem[r1] -= mem[r2];
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[SUB ]:\t %s = %s-%s\n",r1,r1,r2);
    return r1;
}

static char* mock_mul(char* r1, char* r2, FILE* out_file)
{
    mem[r1] *= mem[r2];
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[MUL ]:\t %s = %s*%s\n",r1,r1,r2);
    return r1;
}

static char* mock_div(char* r1, char* r2, FILE* out_file)
{
    mem[r1] /= mem[r2];
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[DIV ]:\t %s = %s/%s\n",r1,r1,r2);
    return r1;
}

static char* mock_lt(char* r1, char* r2, FILE* out_file)
{
    mem[r1] = (mem[r1] < mem[r2]);
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[LT  ]:\t %s = %s < %s\n",r1,r1,r2);
    return r1;
}

static char* mock_le(char* r1, char* r2, FILE* out_file)
{
    mem[r1] = (mem[r1] <= mem[r2]);
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[LE  ]:\t %s = %s <= %s\n",r1,r1,r2);
    return r1;
}

static char* mock_gt(char* r1, char* r2, FILE* out_file)
{
    mem[r1] = (mem[r1] > mem[r2]);
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[GT  ]:\t %s = %s > %s\n",r1,r1,r2);
    return r1;
}

static char* mock_ge(char* r1, char* r2, FILE* out_file)
{
    mem[r1] = (mem[r1] <= mem[r2]);
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[GE  ]:\t %s = %s >= %s\n",r1,r1,r2);
    return r1;
}

static char* mock_eq(char* r1, char* r2, FILE* out_file)
{
    mem[r1] = (mem[r1] == mem[r2]);
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[EQUA]:\t %s = %s == %s\n",r1,r1,r2);
    return r1;
}

static char* mock_ne(char* r1, char* r2, FILE* out_file)
{
    mem[r1] = (mem[r1] != mem[r2]);
    mem[r2] = 0;
    reg_free(r2);
    fprintf(out_file,"[NE  ]:\t %s = %s != %s\n",r1,r1,r2);
    return r1;
}

static char* mock_lt_j(char* r1, char* r2, char* l, FILE* out_file)
{
    // TODO : add mock processing
    reg_free(r1);
    reg_free(r2);
    fprintf(out_file,"[JUMP]:\t %s if %s < %s\n",l,r1,r2);
    return l;
}

static char* mock_le_j(char* r1, char* r2, char* l, FILE* out_file)
{
    // TODO : add mock processing
    reg_free(r1);
    reg_free(r2);
    fprintf(out_file,"[JUMP]:\t %s if %s <= %s\n",l,r1,r2);
    return l;
}

static char* mock_gt_j(char* r1, char* r2, char* l, FILE* out_file)
{
    reg_free(r1);
    reg_free(r2);
    fprintf(out_file,"[JUMP]:\t %s if %s > %s\n",l,r1,r2);
    return l;
}

static char* mock_ge_j(char* r1, char* r2, char* l, FILE* out_file)
{
    reg_free(r1);
    reg_free(r2);
    fprintf(out_file,"[JUMP]:\t %s if %s >= %s\n",l,r1,r2);
    return l;
}

static char* mock_eq_j(char* r1, char* r2, char* l, FILE* out_file)
{
    reg_free(r1);
    reg_free(r2);
    fprintf(out_file,"[JUMP]:\t %s if %s == %s\n",l,r1,r2);
    return l;
}

static char* mock_ne_j(char* r1, char* r2, char* l, FILE* out_file)
{
    reg_free(r1);
    reg_free(r2);
    fprintf(out_file,"[JUMP]:\t %s if %s != %s\n",l,r1,r2);
    return l;
}

static char* mock_jump(char* l, FILE* out_file)
{
    fprintf(out_file,"[JUMP]:\t %s\n",l);
    return l;
}

static char* mock_zero_j(char* r, char* l, FILE* out_file)
{
    fprintf(out_file,"[JPZR]:\t %s if %s == 0\n",l,r);
}

static char* mock_label(char* l, FILE* out_file)
{
    fprintf(out_file,"[LBEL]:\t %s: <- label\n",l);
    return l;
}

static char* mock_var(char* var, FILE* out_file)
{
    // just allocate buffer for var
    string s_var(var);
    g_variable[s_var] = 0;

    fprintf(out_file,"[DECL]:\t %s\n",var);
    return var;
}

static char* mock_store(char* var, char* r, FILE* out_file)
{
    string s_var(var);
    if (g_variable.find(s_var) == g_variable.end()) {
        fprintf(out_file,"[FUCK]:\t %s not found\n",var);
        return NULL;
    }
    g_variable[s_var] = mem[r];
    reg_free(r);

    fprintf(out_file,"[STOR]:\t %s = %s(%d)\n",var,r,mem[r]);
    return var;
}

static char* mock_load_var(char* var, FILE* out_file)
{
    string s_var(var);
    char* r = reg_alloc();
    mem[r] = g_variable[s_var];
    fprintf(out_file,"[LOAD]:\t %s = %s(%d)\n",r,var,g_variable[s_var]);
    return r;
}

