
#ifndef BILL_H_
#define BILL_H_

int PSS_GetCredit(char const* section, char const* name, std::string& strErr);

class CPAI {
public:
    CPAI();
    ~CPAI() {}

    int Login(char const* name, char const* pwd);       // the return value is modified: 10000 success, 10001 failure, 10004 connection failure
    std::string GetPassport();

    std::string LastError();    //  add by jampe

private:
};

#endif