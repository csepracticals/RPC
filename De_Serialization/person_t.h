#ifndef __person_t__
#define __person_t__


/*Forward Declaration*/
typedef struct _person_t_ person_t;

typedef struct _company_t_ {
    char comp_name[32];
    int emp_strength;
    person_t * CEO;
} company_t;


struct _person_t_ {
    unsigned int vehicle_nos[4];
    int age;
    int * height;
    unsigned int acc_nos_count;
    unsigned int * acc_nos;
    unsigned int * last_salary_amounts[12];
    char name[32];
    company_t company;
    unsigned int prev_employers_count;
    company_t * prev_employers;
    company_t dream_companies[3];
    unsigned int friends_count;
    struct _person_t_ * friends;
    struct _person_t_ * CEO;
    struct _person_t_ * administrative_staff[12];
};


#endif

