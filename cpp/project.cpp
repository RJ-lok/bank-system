#include<iostream>
#include<iomanip>
using namespace std;
class Bank
{
 private:
    long account_number;
    string customer_name;
    string account_type;
    int    customer_id;
    string address;
    double mobile;
    float balance;
public:
    Bank()
    {

    }
    Bank(long ac_no,string name,string type,int id,string address,double mobile,float balance){
        this->account_number=ac_no;
        this->customer_name=name;
        this->account_type=type;
        this->customer_id=id;
        this->address=address;
        this->mobile=mobile;
        this->balance=balance;
    }
    void create_account(){
        account_number=867578622953+5;
        customer_id=3155+5;
        cout<<"Enter Your Name...";
        cin>>customer_name;
        cout<<endl<<"Which Type Of Account You Like Currrent / Saving....";
        cin>>account_type;
        cout<<endl<<"Enter Mobile Number...";
        cin>>mobile;
        cout<<endl<<"Enter Address.....";
        cin>>address;
        cout<<"Enter Amount for Deposit...";
        cin>>balance;
    
    }
    void view_balance(){
        cout<<"Hello "<<customer_name<<endl;
        cout<<"   Your Aviable is:- "<<balance<<endl;
    }
    void chenge_mobile()
    {
        long number;
        cout<<"Enter Your New mobile Number....";
        cin>>number;
        mobile=number;
        cout<<"Your Mobile Number has Been Chenge...";
    }
    void tranfer(Bank b2)
    {
        long ac_no;
        string name;
        float amount;
        cout<<"Enter Reciver Account Number...";
        cin>>ac_no;
        cout<<endl<<"Enter Reciver Name...";
        cin>>name;
        if(ac_no==b2.account_number && b2.customer_name==name)
        {
            cout<<"Enter send Amount...";
            cin>>amount;
            if(balance>=amount){
                balance=balance-amount;
                b2.balance=b2.balance+amount;
                cout<<"Money Tranfer Successfully..."<<endl;

            }
            else{
                cout<<"ineffisent balance..."<<endl;
            }
        }
        else
        {
            cout<<"invalid account number...."<<endl;
        }

    }
};

int main()
{
    int choice;
    Bank b1;
    Bank b2(867578622953,"jagdish","saving",3155,"pune,alandi",9921233155,10000);
    while(choice !=5)
    {
        cout<<endl<<"Select Your Work"<<endl<<endl;
        cout<<"1. for create account"<<endl;
        cout<<"2. for check balance"<<endl;
        cout<<"3. for chenge mobile number"<<endl;
        cout<<"4. for tranfer money"<<endl<<endl;
        cout<<"enter your work.....";
        cin>>choice;
        switch(choice)
        {
            case 1:
                    b1.create_account();
                    break;
            case 2:
                    b1.view_balance();
                    break;

            case 3:
                    b1.chenge_mobile();
                    break;

            case 4:
                    b1.tranfer(b2);
                    break;

            default:
                cout<<"enter vaild choice......"<<endl;
                break;

        }


    }
   
}