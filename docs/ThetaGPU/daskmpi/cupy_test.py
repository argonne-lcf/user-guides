import cupy
import dask.array as da
import time

size = 100000

def time_usage(func):
    def wrapper(*args, **kwargs):
        beg_ts = time.time()
        retval = func(*args, **kwargs)
        end_ts = time.time()
        print("elapsed time: %f" % (end_ts - beg_ts))
        return retval
    return wrapper


def with_gpu():
    # with GPU
    rs = da.random.RandomState(RandomState=cupy.random.RandomState)

    x = rs.normal(10, 1, size=(size, size), chunks=(5000, 5000))    
    #print(x.nbytes / 1e9)  # 80 GB
    # 80.0
    
    @time_usage
    def _():
        (x + 1)[::2, ::2].sum().compute()
    _()


def without_gpu():
    # without GPU
    rs = da.random.RandomState()
    
    x = rs.normal(10, 1, size=(size, size), chunks=(5000, 5000))
    #print(x.nbytes / 1e9)  # 80 GB
    # 80.0
    
    @time_usage
    def _():
        (x + 1)[::2, ::2].sum().compute()
    _()


def main():
    print("\nOperation on dask.array (from https://blog.dask.org/2019/01/03/dask-array-gpus-first-steps):")

    print('with gpus')
    with_gpu()

    print('\nwithout gpus')
    without_gpu()


if __name__ == "__main__":
    main()

