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
	static std::string DeclSlice(){
		std::string dslice = 
		"int main(){\n"
			"std::string x;\n"
			"std::string b(\"abc\");\n"
			"const static std::string y = x + b;\n"
			"std::string str1, str2, str3 = \"abc\";\n"
			"std::string str4(b, 5), str6(str4);\n"
			"std::map<int, int>* mp1 = new std::map<int, int>();\n"
			"std::string& str5 = str4;\n"
		"}\n";
		return dslice;
	}
	static std::string ExprSlice(){
		std::string eslice=
		"int main(){\n"
			"int var1, temp, temp2;\n"
			"var1 = var1 + temp - temp2;\n"
		"\n"
			"if(var1 == temp){\n"
				"foo(temp + temp2, var1 + var1);\n"
			"}\n"
			"while(var1 != temp){\n"
				"for(int var = 0; var < temp; ++var){\n"
					"std::cout<<var1+temp2+var;\n"
				"}\n"
			"}\n"
			"return temp + temp2;\n"
		"}\n";
		return eslice;
	}
};