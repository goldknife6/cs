var int a = 1;
var bool bbb = true;

def fib(int a) int {
	if (a == 0)
		return 0;
	if (a == 1)
		return 1;
	else
		return [a-1]fib + [a-2]fib;
}


def print_string(string s) void {
	[s]_buildin_printstring_;
	return;
}

def print_int(int v) void {
	[v]_buildin_printint_;
	return;
}

def open(string filename) file {
	return [filename]_buildin_openfile_;
}

def read(file file) string {
	return [file]_buildin_readfile_;
}

def main() void {
	var file f = null;
	var string con = "";
	$a = [0]fib;
	$f = ["main.c"]open;
	$con = [f]read;

	[con]print_string;
	return;
}
