use std::mem::MaybeUninit;

use criterion::{Criterion, criterion_group, criterion_main};
use nix::libc;
use nix::libc::clockid_t;

fn b(clock_id: clockid_t) {
    let mut c_time: MaybeUninit<libc::timespec> = MaybeUninit::uninit();
    let ret = unsafe { libc::clock_gettime(clock_id, c_time.as_mut_ptr()) };
}

fn criterion_benchmark(c: &mut Criterion) {
    c.bench_function("CLOCK_MONOTONIC", |ben| ben.iter(|| b(libc::CLOCK_MONOTONIC)));
    c.bench_function("CLOCK_MONOTONIC_COARSE", |ben| ben.iter(|| b(libc::CLOCK_MONOTONIC_COARSE)));
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);