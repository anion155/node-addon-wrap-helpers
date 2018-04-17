const assert = require('assert');

describe('addon', () => {
  const addon = require('node-qbs')('NAWHTests');
  describe('require', () => {
    it('should be { A: class(), blah: new A(), BC: class(), blah2: new BC() }', () => {
      assert(typeof addon === 'object');
      assert(typeof addon.A === 'function');
      assert(typeof addon.BC === 'function');
      assert(addon.blah instanceof addon.A);
      assert(addon.blah2 instanceof addon.BC);
    });
  });
  describe('constructors', () => {
    it('should normally construct: (), (int), (double), (string), (double, int)', () => {
      assert.doesNotThrow(() => {
        new addon.A();
        new addon.A(1);
        new addon.A(1.0);
        new addon.A("blah");
        new addon.A(1.0, 3);
      });
    });
    it('should not contruct: (int, string)', () => {
      assert.throws(() => {
        new addon.A(1, "vah");
      });
    });
  });
  describe('methods', () => {
    let a = new addon.A();
    let bc = new addon.BC();
    it('should have methods: hello, hello2, world, world2, lambda, lambda2', () => {
      assert.equal(a.hello(), "hello");
      assert.equal(a.hello2(10), "hello2 10");
      assert.equal(a.world(), "world");
      assert.equal(a.world2(15), "world2 15");
      assert.equal(a.lambda(), "lambda");
      assert.equal(a.lambda2(20), "lambda2 20");
    });
  });
});