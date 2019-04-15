#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <sys/stat.h>

using namespace std;
struct stat sb;
//you can easily add more hashes support, BUT make sure its size is more than 10, otherwise you will just have to edit the addLookUp function on your own, like if i==3,etc
const string arr[]={"md5","sha","all"};
#define arrsize 2
//note that all is not a hash, it is there because the structure of the program asks for it
//also note arrsize is 2 not 3!! if i added on more hash then it would be 3, also take note that 'all' will always be in the last

const string folder[]={"MD5","SHA"};
const int hlength[]={3,3};
const int hsizes[]={32,40};
//wait why is one lowercase other uppercase? See addLookUp function if you want to know why
/*** THESE FUNCTION AT THE TOP ARE THE ERROR FUNCTIONS ***/
inline void nfns(){
  cout<<"ERROR! No filename specified!\n";
  exit(1);
}
inline void fdse(string filename){
  cout<<"ERROR! The file \'"<<filename<<"\' doesn't exists!\n";
  exit(1);
}
/**ERROR FUNCTIONS ENDS HERE**/
int isDirectory(string pathname){
  if (stat(pathname.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    return true;
  else
    return false;
}
bool isArgument(char *str){
  string s=str;
  if (str[0]=='-')
    return true;
  else
    return false;
}
bool strContainsOnly(string s, char c){
  /*This function will check if a string s is repeated array of character c only*/
  bool state=true;
  for (int i=0;i<s.length();++i)
    if (s[i]!=c)
      state=false;
  return state;
}
bool strContains(string s, char c){
  /*This function will check if a string s contains character c*/
  bool state=false;
  for (int i=0;i<s.length();++i)
    if (s[i]==c)
      state=true;
  return state;
}
bool isBlankSpace(string s,bool hasBlankSpace){
  /*
    This function will check if a string is a blank space and also if it contains blank space
    Working: First check if the string contains only white spaces; if not  check if it has blankspace
    If yes then check if it contains white space if yes than check if hasblankSpace is true. If yes
    return false else return true. i.e. hello world for hasBlankSpace=true will return false
  */
  if ((strContainsOnly(s,'\t') or s=="\0" or strContainsOnly(s,'\0') or strContainsOnly(s,'\v') or strContainsOnly(s,32) or s==" " or s=="" or s=="\n" or s=="\v" or s=="\t")){
    return true;
  }
  else
    if ((strContains(s,'\t') or strContains(s,'\0') or strContains(s,'\n') or strContains(s,'\v') or strContains(s,32))){
      if (hasBlankSpace)
        return false;
      else
        return true;}
    else{
      return false;cout<<"true";
    }
}
string _hash(string s,int i){
  /*
    This function will return md5 hash of string s
    0-MD5, 1-SHA
  */
  string str="echo -n \"" +s+ "\" | " + arr[i].c_str() + "sum | tr -d \" -\" > \"./MD5/generated_temporary_file.txt\"";
  system(str.c_str());
  ifstream file ("./MD5/generated_temporary_file.txt");
  getline(file,s);
  file.close();
  system("rm ./MD5/generated_temporary_file.txt");
  return s;
}
string getAllArguments(int argc,char *args[],int def=2){
  /*
    This function will get the entire string after the args[def]
  */
  string str="";
  for (int i=def;i<argc;++i){
     str=str+string(args[i]);
     if (i+1!=argc)
      str=str+char(32);}
  return str;
}
int noOfLines(string filename){
  /*
    The use of this func is to count the no of lines of a file- don't use it often :()
  */
  ifstream file (filename.c_str());
  int nol;
  while (file.good()){
     getline(file,filename);
     ++nol;
   }
  return nol-1;
}
bool checkFile(string filename){
  bool b=false;
  if (filename=="")
    nfns();
  else if (ifstream (filename.c_str()).fail())
      fdse(filename);
  else b=true;
  return b;
}
void printHashFile(string filename,int hash,bool hashSpace,bool verbose,bool showhash=false){
  /*
    This func will print all the md5 hash of a string in a file and will print md5 hash of
    string containing white spaces (but not ONLY white spaces) when hashSpace is true for ex: hello world
  */
  ifstream file (filename.c_str());
  string str;
  if (verbose){
    cout<<'+'<<string(65-hsizes[hash],'-')<<"+"<<string(2+hsizes[hash],'-')<<'+'<<endl;
    cout<<"| Text"<<string(60-hsizes[hash],32);
    cout<<"|"<<char(showhash?char(32):0)<<(showhash?folder[hash]:"")<<" Hash"<<string(hsizes[hash]-7+(!showhash?hlength[hash]+1:0),32)<<"|"<<endl;
    cout<<'+'<<string(65-hsizes[hash],'-')<<"+"<<string(2+hsizes[hash],'-')<<'+'<<endl;
  }
  while (file.good()){
    getline(file,str);

    if (not isBlankSpace(str,hashSpace)){
      if (verbose){  
        //if (showhash) cout<<folder[hash];
        cout<<"| "<<str;
        int limit=65-hsizes[hash];
        if (str.length()<limit)
           cout<<string(limit-str.length()-1,32);
        else
          cout<<char(32);}
      cout<<(verbose?"| ":"")<<_hash(str,hash)<<(verbose?" |":"")<<endl;}
    }
    if (verbose)cout<<'+'<<string(65-hsizes[hash],'-')<<"+"<<string(2+hsizes[hash],'-')<<'+'<<endl;
}
bool addLookUp(string s, int i){
  /* Add Hash LookUp record + Check if the record already exists*/
  string hashL=_hash(s,i);
  //if you have more storage than "5TB", then u can substr to bigger filename though not recommended (by me ^_-) 
  string dir="./"+folder[i]+"/"+hashL.substr(5,2);
  //if we keep on converting into uppercase for every single record, then the perfomance can be slow
  // for some thousand records this may be insignificant but when you cross ten-thousand then you will feel the delay, even if its 1s, we cannot afford it
  if (!isDirectory(dir))
    system(("mkdir "+dir).c_str());
  dir+="/"+hashL.substr(7,3);
  if (!isDirectory(dir))
    system(("mkdir "+dir).c_str());
  bool hex=false;//HashEXists or not - to prevent duplicate records
  string str;
  ifstream fin((dir+"/"+hashL.substr(0,2)).c_str());
  while (fin.good()){
    getline(fin,str);
    if (str.length()>hsizes[i])//it will be more than hsizes[i] but just in case if it isn't (suppose someone edits the file)
      if (str.substr(0,hsizes[i])==hashL){
        hex=true;
        break;}
  }
  
  //why didn't I use check funtion? for performance reasons, calling a function within another funtion which is also in another function, this can affect performance
  //consider the fact that this function is going to be called several thousand times, maybe even MILLION TIMES!!! A small saving of 0.0001s per record can save HOURS for large amount of records
  if (not hex){
    ofstream fout((dir+"/"+hashL.substr(0,2)).c_str(),ios::app);
    fout<<hashL<<s<<endl;
  }
  return hex;
}
void addHashFile(string filename,int hash,bool hashSpace,bool verbose=true){
  /*
    This func will get  all the md5 hash of a string in a file and will add it to lookup database
  */
  ifstream file (filename.c_str());
  string str;
  if (verbose and checkFile(filename))
    while (file.good()){
      getline(file,str);
      if (not isBlankSpace(str,hashSpace))
        if (hash!=arrsize){
          if(addLookUp(str,hash)&&verbose)
            cout<<"ERROR! The "<<folder[hash]<<" hash for \'"<<str<<"\' already exists!\n";
          }
        else{
          for (int j=0;j<arrsize;++j)
            if(addLookUp(str,j)&&verbose){
              cout<<"ERROR! The "<<folder[j]<<" hash for \'"<<str<<"\' already exists!\n";
            }
          }
    }
}
bool decrypt(string s,int i,string &result){
  //will decrypt a hash if it exists (return 1) and store it to result or simply return 0 if it doesn't
  int hex=2;
  if (s.length()<hsizes[i]) hex=0;//what if the user is trying to fool us and doesn't give us a proper hash
  if (hex){
    string dir="./"+folder[i]+"/"+s.substr(5,2)+"/"+s.substr(7,3);
    if (!isDirectory(dir))
      hex=0;
    else{
      ifstream fin((dir+"/"+s.substr(0,2)).c_str());
      string str;
      while (fin.good()){
        getline(fin,str);
        if (str.length()>hsizes[i])//it will be more than hsizes[i] but just in case if it isn't (suppose someone edits the file)
          if (str.substr(0,hsizes[i])==s){
            hex=1;
            result=str.substr(hsizes[i]);
            break;}
          }
        }}
  if (hex==2) hex=0;
  return hex;
}
void decryptFile (string filename,int hash,bool verbose=true,bool showhash=false){
  ifstream file (filename.c_str());
  string str;
  
  if (verbose){
    //cout<<'+'<<string(82-hsizes[hash],'-')<<"+"<<string(65-hsizes[hash],'-')<<'+'<<endl;
    cout<<'+'<<string(hsizes[hash]+2,'-')<<"+"<<string(65-hsizes[hash],'-')<<'+'<<endl;
    cout<<"|"<<char(showhash?char(32):0)<<(showhash?folder[hash]:"")<<" Hash"<<string(hsizes[hash]-7+(!showhash?hlength[hash]+1:0),32)<<"|";
    cout<<" Text"<<string(60-hsizes[hash],32)<<"|"<<endl;
    cout<<'+'<<string(hsizes[hash]+2,'-')<<"+"<<string(65-hsizes[hash],'-')<<'+'<<endl;    
  }
  while (file.good()){
    getline(file,str);
    if (not isBlankSpace(str,true)){
      int pos;
      if (str.find_first_of(" ")>str.length())
          pos=str.length();
      else
          pos=str.find_first_of(" ");
      str=str.substr(0,pos);
      if (str.length()<=hsizes[hash]){      
      if (verbose){  
        //if (showhash) cout<<folder[hash];
        cout<<"| "<<str;
        }
      int limit=hsizes[hash];
      if (str.length()<limit)
           cout<<string(limit-str.length(),32);
      cout<<(verbose?" | ":"");
      string res;
      
      if(decrypt(str,hash,res))
        res='\''+res+'\'';
      else res=" NOT FOUND";
      cout<<res;
      limit=65-hsizes[hash];
      if (res.length()<limit)
           cout<<string(limit-res.length()-1,32);
        else
          cout<<char(32);
      cout<<(verbose?"|":"")<<endl;}
    }}
    if (verbose)
      cout<<'+'<<string(hsizes[hash]+2,'-')<<"+"<<string(65-hsizes[hash],'-')<<'+'<<endl;      
}
inline void hfile(string filename,int i,bool b1,bool b2){
  if ((b2&&checkFile(filename)) or (!b2))
    if (i!=arrsize)
      printHashFile(filename,i,b1,b2);
    else
      for (int j=0;j<arrsize;++j)
        printHashFile(filename,j,b1,b2,true);
}
void getWV(string a1, string a2,int &ac,bool &w0,bool &v0){
    //we will break cases, their are 9 of them, we will group them together
    int w=1,v=1;
    if ((a1[0]=='+' or a1[0]=='-') and (a1.substr(1)=="w" or a1.substr(1)=="v")){
        if (a1.substr(1)=="w")
            w=a1[0]-45;
        else 
            v=a1[0]-45;
        ac+=1;
        if (a2[0]=='+' or a2[0]=='-'){
            ac+=1;
            if (a2.substr(1)=="w"&&v!=1)
                w=a2[0]-45;
            else if (a2.substr(1)=="v"&&w!=1)
                v=a2[0]-45;
            else ac-=1;
        }
    }
    w0=w;v0=v;
    if (w==1)
      w0=false;
} 
void getWV(string a,int &ac,bool &verbose){
  int v=1;
  if (a=="-v" or a=="+v"){
    v=a[0]-45;
    ac+=1;
  }
  else v=1;
  verbose=v;
}
void help(char* str){
  printf("Usage: %s [Option] [Parameters] <input-stream>\n\nCracks a ",str);
  for (int i=0;i<arrsize;++i)
    cout<<folder[i]<<(i!=arrsize-1?",":"");
  cout<<" hash in no time using existing lookup table + MUCH MORE\n\n"
      <<"Below are the options which you can use:-\n\n"
      <<"1. CONVERT TO HASH: If you wish to convert a text in the stream \n"
      <<string(20,32)<<"then use this option. For example-\n\n";
  printf("   > %s -md5 hello\t would print MD5 hash of \"hello\"\n",str);
  printf("   > %s -md5 --file hello.txt\t would print MD5 hash of all lines in \"hello.txt\"\n",str);  
  exit(0);
}
int main(int argc,char *args[]){
    
    for (int i=0;i<arrsize;++i)
      if(!isDirectory(folder[i]))
        system(("mkdir "+folder[i]).c_str());
    if (argc==1)
      printf("Usage: %s [Options] <input-stream>\n",args[0]);
    else{
      if (isArgument(args[1])) {
        if (argc>2){
          for (int i=0;i<arrsize+1;++i){
            if (not strcmp(args[1],("-"+arr[i]).c_str()))
              //if not means if args[1]==md5 then check if the input-stream is a file or string
              //filew means that strings containing white spaces will not be ignored
              if (strcmp(args[2],"--file")==0){
                bool w,v;int ag=3;
                if (argc==3)
                  nfns();
                if (argc==4)
                  getWV(args[3],"",ag,w,v);                
                else
                  getWV(args[3],args[4],ag,w,v);
                hfile(getAllArguments(argc,args,ag),i,w,v);
              }
                /*What I did previously was 
                if (strcmp(args[3],"-w")) //if not -w
                  hfile(argc,args,3+((strcmp(args[3],"+w")==0)?1:0),i,true);
                else
                  hfile(argc,args,4,i,false);*/
              else
                if (i!=arrsize)
                  cout<<_hash(getAllArguments(argc,args),i)<<endl;
                else
                  for(int j=0;j<arrsize;++j)
                    cout<<folder[j]<<": "<<_hash(getAllArguments(argc,args),j)<<endl;
            else if (not strcmp(args[1],("-add["+arr[i]+"]").c_str())) {
              if (strcmp(args[2],"--file")==0){
                bool w0=1,v0=1;int ag0=3;
                if (argc==3) nfns();
                else if (argc==4)
                  getWV(args[3],"",ag0,w0,v0);
                else
                  getWV(args[3],args[4],ag0,w0,v0);                
                addHashFile(getAllArguments(argc,args,ag0),i,w0,v0);}
              else{
                if (i!=arrsize){
                  if(addLookUp(getAllArguments(argc,args),i))
                    cout<<"ERROR! The "<<folder[i]<<" hash for \'"<<getAllArguments(argc,args)<<"\' already exists!\n";
                }
                else{
                  for (int j=0;j<arrsize;++j)
                    if(addLookUp(getAllArguments(argc,args),j)){
                      cout<<"ERROR! The "<<folder[j]<<" hash for \'"<<getAllArguments(argc,args)<<"\' already exists!\n";
                    }
                  }
                }
            }
            else if (not strcmp(args[1],("-decrypt["+arr[i]+"]").c_str())){
              if (not strcmp(args[2],"--file")){
                bool v=true;int ag=3;
                if (argc==3) nfns();
                else getWV(args[3],ag,v);
                //if (ag==argc) nfns();, not needed as checkFile will perform the check
                if (i!=arrsize){
                  if (checkFile(getAllArguments(argc,args,ag)))
                    decryptFile(getAllArguments(argc,args,ag),i,v);}
                else{
                  for (int j=0;j<arrsize;++j)
                    if (checkFile(getAllArguments(argc,args,ag)))
                      decryptFile(getAllArguments(argc,args,ag),j,v,true);                  
                }
              }
              else{
                string result;
                bool v;int ag=2;
                getWV(args[2],ag,v);
                if (i!=arrsize){
                  bool b=decrypt(getAllArguments(argc,args,ag),i,result);
                  if (!b&&v)
                    cout<<"Sorry! The Hash doesn't exist in the database :(\n";
                  else
                    if (b) 
                      cout<<(v?"Hash Found: '":"")<<result<<(v?"\'\n":"\n");}
                else
                  for (int j=0;j<arrsize;++j){
                    bool b=decrypt(getAllArguments(argc,args,ag),j,result);
                    if (!b&&v)
                      cout<<folder[j]<<" Hash Not Found!\n";
                    else
                      if (b)
                        cout<<(v?folder[j]+" Hash Found: '":"")<<result<<(v?"\'\n":"\n");
                      }
                    }
                  }
                }
          
          }
         else {
           if (strcmp(args[1],"--help")==0 or strcmp(args[1],"-h")==0){
             help(args[0]);}
          else cout<<"PARSING ERROR! Please check arguments again!\n";
         }
       }
       else cout<<"INVALID SYNTAX! Please check arguments again!\n";
     }
   }
