class ExampleError(BaseException):
    pass

a = 1
b = 0
if b == 1:
    raise ExampleError("b==0")

print("hello")