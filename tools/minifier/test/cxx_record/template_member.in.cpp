struct A {
	template <typename T>
	int foo(){ return sizeof(T); }
	template <typename T>
	int bar(){ return sizeof(T); }
};
int main(){
	A a;
	return a.foo<int>();
}
