struct A {
public:
	int a;
protected:
	int b;
private:
	int c;
public:
	A() : a(0), b(0), c(0) { }
};
int main(){
	A a;
	return a.a;
}
