struct X {
	static const int x = 10;
	static const int y = 20;
};
struct A {
	int a = X::x;
};
int main(){
	A a;
	return a.a;
}
