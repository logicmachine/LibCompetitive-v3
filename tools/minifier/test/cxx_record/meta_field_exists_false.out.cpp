template <typename T>
struct A {
private:
	static short check(...);
public:
	static const int value = sizeof(decltype(check(T())));
};
struct C { };
int main(){
	return A<C>::value;
}
