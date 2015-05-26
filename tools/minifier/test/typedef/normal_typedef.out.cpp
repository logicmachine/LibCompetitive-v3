typedef int hoge_type;
typedef int fuga_type;
int main(){
	hoge_type hoge = 0;
	return static_cast<fuga_type>(hoge);
}
