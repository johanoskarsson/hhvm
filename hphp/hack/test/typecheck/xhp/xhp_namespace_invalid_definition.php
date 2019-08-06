<?hh // strict

namespace foo;

// This kind of nested namespace is not allowed.

class :herp:derp {
	// From https://gist.github.com/fredemmott/1e65842fadb9e66e2359
	//
	// - \xhp__herp_derp - BC break
	// + PARSE ERROR

	// Arguments for making it an error instead of \foo\herp\xhp__derp:
	//  - allowing two namespace syntaxes is weird. We allow it in the global namespace for backwards compatibility
	//  - it looks like this creates a class in the 'foo' namespace. It would actually make a class in the \foo\herp namespace
	//  - it's currently pretty broken: you can define the class, but you can't reference (or extend) :x:element and friends 
	// Arguments for allowing it:
	//  - creating :foo and :foo:helper is currently common (however, :foo and :foo-helper is also perfectly valid)
	//  - it's a bit weird to allow it in the global namespace, but not others
	//  - possibly more appropriate as a lint error than a parser error
	//
	// We aren't entirely sure what the right approach is. We decided to make it an error as this lets us revisit it later;
	// if we allowed it, we can't ban it in the future without breaking people's code.
}
