var m;
var mem8;

function puts(p) {
  var s = '';
  while (mem8[p] != 0)
    s += String.fromCharCode(mem8[p++]);
  print(s);
}

function writeByte(b) {
  var s = b.toString();
  while (s.length < 3) {
    s = " " + s;
  }
  write(s);
}

function flipBuffer(p, w, h) {
  print("P3");
  print(w + " " + h);
  print("255");
  print("# This is a PPM file, redirect stdout to view.");
  for (var j = 0; j < h; j++) {
    for (var i = 0; i < w; i++) {
      if (i != 0) write("  ");
      writeByte(mem8[p++]);
      write(" ");
      writeByte(mem8[p++]);
      write(" ");
      writeByte(mem8[p++]);
      p++;
    }
    print();
    print();
  }
  print("# This is a PPM file, redirect stdout to view.");
}

var ffi = {
  print: print,
  puts: puts,
  flipBuffer: flipBuffer
};

m = WASM.instantiateModule(readbuffer(arguments[0]), ffi);
mem8 = new Uint8Array(m.memory);
m.main();
