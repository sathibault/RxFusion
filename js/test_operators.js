var assert = require('assert');
var rx = require('./index.js');

var groups = [];
function testgroup(title,subtitle,setup) {
    groups.push({title: title, subtitle: subtitle, setup: setup});
}

var cases;
var failed;
function testcase(title, run) {
    cases.push({title: title, run: run});
}

function runcase(title, subtitle, c) {
    console.log('\t' + c.title);
    try {
	return c.run().catch(err => {
	    console.log(err.stack);
	    console.log(title + ' ' + subtitle + ' ' + c.title + ' failed: ' + err.message);
	    failed+=1;
	});
    } catch(err) {
	console.log(err.stack);
	console.log(title + ' ' + subtitle + ' ' + c.title + ' failed: ' + err.message);
	failed+=1;
    }
}

function caserunner(g, ary, i) {
    return function() {
	return runcase(g.title, g.subtitle, ary[i]);
    };
}

function rungroup(g) {
    console.log(g.title);
    console.log(g.subtitle);
    failed = 0;
    cases = [];
    g.setup();
    var pred = runcase(g.title, g.subtitle, cases[0]);
    var i = 1;
    while (i < cases.length) {
	pred = pred.then(caserunner(g, cases, i));
	i += 1;
    }
    return pred.then(x => console.log(failed + ' failed - ' + g.title + ' ' + g.subtitle));
}

function grouprunner(ary, i) {
    return function() {
	return rungroup(ary[i]);
    }
}

function runtests() {
    var pred = rungroup(groups[0]);
    var i = 1;
    while (i < groups.length) {
	pred = pred.then(grouprunner(groups, i));
	i += 1;
    }
    pred.then(x => console.log('done'));
}

testgroup('operator test','filtering', () => {
    testcase('drop', () => {
	var r = new Range(1,8);
	var out = new Doc();
	return rxbase.test()(r >> Drop(3) >> out).then(x => {
	    assert.equal(out.text(), '4 5 6 7 8');
	});
    });

    testcase('take', () => {
	var r = new Range(1,8);
	var out = new Doc();
	return rxbase.test()(r >> Take(3) >> out).then(x => {
	    assert.equal(out.text(), '1 2 3');
	});
    });

    testcase('dedup', () => {
	var r = new Range(1,3);
	var out = new Doc();
	var mix = rx()(r + r + r);
	return rxbase.test()(mix >> Dedup() >> out).then(x => {
	    assert.equal(out.text(), '1 2 3');
	});
    });

    testcase('expiring dedup', () => {
	var r = new Range(0,11);
	var out = new Doc();
	var tmp = rx()(r >> Iterate(100) >> Map(x => Math.floor(x/4)));
	return rxbase.test()(tmp >> Dedup(250) >> out).then(x => {
	    assert.equal(out.text(), '0 0 1 1 2 2');
	});
    });

    testcase('over', () => {
	var r = new Range(1,8);
	var out = new Doc();
	return rxbase.test()(r >> Over(5) >> out).then(x => {
	    assert.equal(out.text(), '6 7 8');
	});
    });

    testcase('debounce', () => {
	// 300 5
	// 400 1
	// 620 6 (this one is always off somehow)
	// 800 2
	// 920 7
	// 1200 3
	// 1220 8
	var ra = new Range(1,3);
	var rb = new Range(5,8);
	var out = new Doc();
	var data = rx()(rx()(ra >> Iterate(400)) + rx()(rb >> Iterate(300)));
	return rxbase.test()(data >> Debounce(140) >> out).then(x => {
	    assert.equal(out.text(), '1 6 7 8');
	});
    });

    testcase('throttle', () => {
	// 300 5
	// 400 1
	// 620 6 (this one is always off somehow)
	// 800 2
	// 920 7
	// 1200 3
	// 1220 8
	var ra = new Range(1,3);
	var rb = new Range(5,8);
	var out = new Doc();
	var data = rx()(rx()(ra >> Iterate(400)) + rx()(rb >> Iterate(300)));
	return rxbase.test()(data >> Throttle(500) >> out).then(x => {
	    assert.equal(out.text(), '5 6 3');
	});
    });

    testcase('sample', () => {
	// 300 5
	// 400 1
	// 620 6 (this one is always off somehow)
	// 800 2
	// 920 7
	// 1200 3
	// 1220 8
	var ra = new Range(1,3);
	var rb = new Range(5,8);
	var out = new Doc();
	var data = rx()(rx()(ra >> Iterate(400)) + rx()(rb >> Iterate(300)));
	return rxbase.test()(data >> Sample(500) >> out).then(x => {
	    assert.equal(out.text(), '1 7 8');
	});
    });
});

testgroup('operator test','xOf', () => {
    testcase('sum', () => {
	var r = new Range(1,10);
	var out = new Doc();
	return rxbase.test()(r >> SumOf(3) >> out).then(x => {
	    assert.equal(out.text(), '6 15 24');
	});
    });

    testcase('average', () => {
	var r = new Range(1,10);
	var out = new Doc();
	return rxbase.test()(r >> AverageOf(3) >> out).then(x => {
	    assert.equal(out.text(), '2 5 8');
	});
    });

    testcase('min', () => {
	var r = new Range(1,10);
	var out = new Doc();
	return rxbase.test()(r >> MinOf(3) >> out).then(x => {
	    assert.equal(out.text(), '1 4 7');
	});
    });

    testcase('max', () => {
	var r = new Range(1,10);
	var out = new Doc();
	return rxbase.test()(r >> MaxOf(3) >> out).then(x => {
	    assert.equal(out.text(), '3 6 9');
	});
    });
});

testgroup('operator test', 'xOver', () => {
    testcase('sum', () => {
	var r = new Range(1,9);
	var out = new Doc();
	return rxbase.test()(r >> Iterate(150) >> SumOver(500) >> out).then(x => {
	    assert.equal(out.text(), '6 15 24');
	});
    });

    testcase('average', () => {
	var r = new Range(1,9);
	var out = new Doc();
	return rxbase.test()(r >> Iterate(150) >> AverageOver(500) >> out).then(x => {
	    assert.equal(out.text(), '2 5 8');
	});
    });

    testcase('min', () => {
	var r = new Range(1,9);
	var out = new Doc();
	return rxbase.test()(r >> Iterate(150) >> MinOver(500) >> out).then(x => {
	    assert.equal(out.text(), '1 4 7');
	});
    });

    testcase('max', () => {
	var r = new Range(1,9);
	var out = new Doc();
	return rxbase.test()(r >> Iterate(150) >> MaxOver(500) >> out).then(x => {
	    assert.equal(out.text(), '3 6 9');
	});
    });
});

testgroup('operator test', 'WinX', () => {
    testcase('sum', () => {
	var r = new Range(1,6);
	var out = new Doc();
	return rxbase.test()(r >> WinSum(3) >> out).then(x => {
	    assert.equal(out.text(), '6 9 12 15');
	});
    });

    testcase('average', () => {
	var r = new Range(1,6);
	var out = new Doc();
	return rxbase.test()(r >> WinAverage(3) >> out).then(x => {
	    assert.equal(out.text(), '2 3 4 5');
	});
    });

    testcase('min', () => {
	var r = new Range(1,6);
	var out = new Doc();
	return rxbase.test()(r >> WinMin(3) >> out).then(x => {
	    assert.equal(out.text(), '1 2 3 4');
	});
    });

    testcase('max', () => {
	var r = new Range(1,6);
	var out = new Doc();
	return rxbase.test()(r >> WinMax(3) >> out).then(x => {
	    assert.equal(out.text(), '3 4 5 6');
	});
    });
});

testgroup('operator test','conditionals', () => {
    testcase('IsAtLeast', () => {
	var r = new Range(1,5);
	var out = new Doc();
	return rxbase.test()(r >> IsAtLeast(3) >> out).then(x => {
	    assert.equal(out.text(), 'false false true true true');
	});
    });
});

testgroup('builder tests','all', () => {
    testcase('zip', () => {
	var r = new Range(1,3);
	var out = new Doc();
	var scaled = rx()(r >> Scale(2));
	return rxbase.test()(rx()(r ^ scaled) >> out).then(x => {
	    assert.equal(out.text(), '1,2 2,4 3,6');
	});
    });

    testcase('merge', () => {
	var r = new Range(1,3);
	var out = new Doc();
	var scaled = rx()(r >> Scale(2));
	return rxbase.test()(rx()(r + scaled) >> out).then(x => {
	    assert.equal(out.text(), '2 1 4 2 6 3');
	});
    });
});

runtests();
