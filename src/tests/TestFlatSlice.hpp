#include <string>
struct FlatSlicePrograms{
	static std::string FlatSliceOne(){
		std::string FlatSliceOneV = 
			"void fun(int z){\n"
			"	z++;\n"
			"	return z;\n"
			"}\n"
			"void foo(int &x, int *y){\n"
				"fun(x);\n"
				"y++;\n"
			"}\n"
			"int main(){\n"
				"int sum = 0;\n"
				"int i = 1;\n"
				"while (i<=10){\n"
					"foo(sum, i);\n"
				"}\n"
				"std::cout<<\"i: \"<<i<<\"sum: \"<<sum<<std::endl;\n"
				"std::cout<<fun(i);\n"
				"sum=sum+i;\n"
			"}\n";
		return FlatSliceOneV;
	}

};