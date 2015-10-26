var m;
var mem8;

function puts(p) {
  var s = '';
  while (mem8[p] != 0)
    s += String.fromCharCode(mem8[p++]);
  print(s);
}

ffi = {print: print, puts: puts};
m = WASM.instantiateModule(readbuffer(arguments[0]), ffi);
mem8 = new Uint8Array(m.memory);
m.main();
