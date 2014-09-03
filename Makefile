CFLAGS = \
                 -Wall \
                 -Wextra \
                 -Wformat=2 \
                 -Wno-unused-parameter \
                 -Wno-missing-field-initializers \
                 -Wmissing-include-dirs \
                 -Wfloat-equal \
                 -Wpointer-arith \
                 -Wwrite-strings \
                 -Wshadow \
                 -Wno-pmf-conversions \
                 -O2

all: test_rbtree test_astree

clean:
        @rm -f test_rbtree test_astree iterative recursive

%: %.cpp
        g++ -o $@ $^ $(CFLAGS)
