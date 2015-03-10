'use strict';
(function(){

	var name = 'LestaD';
	var v = version + .4;

	greet(name);

	getVersion(function(v){
		print("my version is: "+v);
	});
})(this);