template <typename T>
struct A {
	void a(){ }
	void b(){ }
	void c(){ }
	void d(){ }
};
int main(){
	A<int> a_int;
	a_int.a();
	A<short> a_short;
	a_short.b();
	A<char> a_char;
	a_char.c();
	return 0;
}
