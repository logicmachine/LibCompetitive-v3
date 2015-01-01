template <typename T>
struct A {
private:
	template <typename U>
	static auto check(const U &x) -> decltype(x.a, int());
	static short check(...);
public:
	static const int value = sizeof(decltype(check(T())));
};
struct B {
	int a;
};
struct C { };
int main(){
	return A<C>::value;
}
