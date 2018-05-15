const assert = require('assert');
let addon = null;

describe('addon', () => {
  describe('require', () => {
    it('should load addon', () => {
      addon = require('node-qbs')('NAWHTests');
    });
    it('addon should be { A: class(), blah: new A(), BC: class(), blah2: new BC() }', () => {
      assert(typeof addon === 'object');
      assert(typeof addon.A === 'function');
      assert(typeof addon.BC === 'function');
      assert(addon.blah instanceof addon.A);
      assert(addon.blah2 instanceof addon.BC);
      assert(addon.blah2 instanceof addon.A);
    });
  });
  describe('constructors', () => {
    it('A should normally construct: (), (int), (double), (string), (double, int)', () => {
      assert.doesNotThrow(() => {
        new addon.A();
        new addon.A(1);
        new addon.A(new addon.A());
        new addon.A(1.0);
        new addon.A("blah");
        new addon.A(1.0, 3);
      });
    });
    it('A should not contruct: (int, string)', () => {
      assert.throws(() => {
        new addon.A(1, "vah");
      });
    });
  });
  describe('methods', () => {
    it('A should have methods: hello, hello2, world, world2, lambda, lambda2', () => {
      let a = new addon.A();
      assert.equal(typeof a.hello, "function");
      assert.equal(typeof a.hello2, "function");
      assert.equal(typeof a.world, "function");
      assert.equal(typeof a.world2, "function");
      assert.equal(typeof a.lambda, "function");
      assert.equal(typeof a.lambda2, "function");
    });
    it('A methods results', () => {
      let a = new addon.A();
      assert.equal(a.hello(), "hello");
      assert.equal(a.hello2(10), "hello2 10");
      assert.equal(a.world(), "world");
      assert.equal(a.world2(15), "world2 15");
      assert.equal(a.lambda(), "lambda");
      assert.equal(a.lambda2(20), "lambda2 20");
    });
  });
  describe('properties', () => {
    it('A should have properties: prop, prop2, prop3, prop4, prop5', () => {
      let a = new addon.A();
      assert.equal(typeof a.prop, "string");
      assert.equal(typeof a.prop2, "number");
      assert.ok(Number.isInteger(a.prop2));
      assert.equal(typeof a.prop3, "object");
      assert.ok(a.prop3 instanceof addon.A);
      assert.equal(typeof a.prop4, "number");
      assert.ok(!Number.isInteger(a.prop4));
      assert.equal(typeof a.prop5, "number");
    });
    it('A props should be changable', () => {
      let a = new addon.A();

      assert.equal(a.prop, "default_prop");
      a.prop = "changed_prop";
      assert.equal(a.prop, "changed_prop");

      assert.equal(a.prop2, 5);
      a.prop2 = 55;
      assert.equal(a.prop2, 55);

      let b = new addon.A();
      assert.equal(a.prop3, a);
      a.prop3 = b;
      assert.equal(a.prop3, b);

      assert.equal(a.prop4, 10.5);
      a.prop4 = 8.24;
      assert.equal(a.prop4, 8.24);

      assert.equal(a.prop5, 53);
      a.prop5 = 28;
      assert.equal(a.prop5, 28);
    });
    it('A init `prop` in constructor', () => {
      let a = new addon.A("on_create");
      assert.equal(a.prop, "on_create");

      a = new addon.A(102);
      assert.equal(a.prop2, 102);

      let b = a; a = new addon.A(b);
      assert.equal(a.prop3, b);

      a = new addon.A(24.5);
      assert.equal(a.prop4, 24.5);

      a = new addon.A(61.48, 47);
      assert.equal(a.prop4, 61.48);
      assert.equal(a.prop5, 47);
    });
  });
});
