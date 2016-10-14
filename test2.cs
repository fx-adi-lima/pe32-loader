using System;

class Test2
{
	static void Main(string[] args)
	{
		Console.WriteLine("Got {0} arguments:", args.Length);
		foreach (string arg in args)
			Console.WriteLine("\t- {0}", arg);
	}
}

