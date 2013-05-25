# Statistics generator script.  A good overnight job.  Then use table.awk
# to convert an output stats file into a timings table.

# General polygon tests
make clean
export MAKEOPTS="-DTIMER -DRANDOM -DSORT"
make

# test all basic algorithms on random polygons
p_test -v 3 -n 50 -i 50 -r 0 -p 1 -d -ABCMPSW > Rr0.sts
p_test -v 4 -n 50 -i 50 -r 0 -p 1 -d -ABCMPSW >> Rr0.sts
p_test -v 10 -n 50 -i 50 -r 0 -p 1 -d -ABCMPSW >> Rr0.sts
p_test -v 20 -n 50 -i 50 -r 0 -p 1 -d -ABCMPSW >> Rr0.sts
p_test -v 50 -n 50 -i 50 -r 0 -p 1 -d -ABCMPSW >> Rr0.sts
p_test -v 100 -n 50 -i 50 -r 0 -p 1 -d -ABCMPSW >> Rr0.sts
p_test -v 1000 -n 50 -i 50 -r 0 -p 1 -d -ABCMPSW >> Rr0.sts

# grid and trapezoid tests at resolution 20
p_test -v 3 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 20 -b 20 > Rr0R20.sts
p_test -v 4 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 20 -b 20 >> Rr0R20.sts
p_test -v 10 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 20 -b 20 >> Rr0R20.sts
p_test -v 20 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 20 -b 20 >> Rr0R20.sts
p_test -v 50 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 20 -b 20 >> Rr0R20.sts
p_test -v 100 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 20 -b 20 >> Rr0R20.sts
p_test -v 1000 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 20 -b 20 >> Rr0R20.sts

# grid and trapezoid tests at resolution 100
p_test -v 3 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 100 -b 100 > Rr0R100.sts
p_test -v 4 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 100 -b 100 >> Rr0R100.sts
p_test -v 10 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 100 -b 100 >> Rr0R100.sts
p_test -v 20 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 100 -b 100 >> Rr0R100.sts
p_test -v 50 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 100 -b 100 >> Rr0R100.sts
p_test -v 100 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 100 -b 100 >> Rr0R100.sts
p_test -v 1000 -n 50 -i 50 -r 0 -p 1 -d -CGT -g 100 -b 100 >> Rr0R100.sts


# test all basic algorithms on regular polygons
p_test -v 3 -n 50 -i 50 -r 1 -p 0 -d -ABCMPSW > Rr1.sts
p_test -v 4 -n 50 -i 50 -r 1 -p 0 -d -ABCMPSW >> Rr1.sts
p_test -v 10 -n 50 -i 50 -r 1 -p 0 -d -ABCMPSW >> Rr1.sts
p_test -v 20 -n 50 -i 50 -r 1 -p 0 -d -ABCMPSW >> Rr1.sts
p_test -v 50 -n 50 -i 50 -r 1 -p 0 -d -ABCMPSW >> Rr1.sts
p_test -v 100 -n 50 -i 50 -r 1 -p 0 -d -ABCMPSW >> Rr1.sts
p_test -v 1000 -n 50 -i 50 -r 1 -p 0 -d -ABCMPSW >> Rr1.sts

# grid and trapezoid tests at resolution 20
p_test -v 3 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 20 -b 20 > Rr1R20.sts
p_test -v 4 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 20 -b 20 >> Rr1R20.sts
p_test -v 10 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 20 -b 20 >> Rr1R20.sts
p_test -v 20 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 20 -b 20 >> Rr1R20.sts
p_test -v 50 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 20 -b 20 >> Rr1R20.sts
p_test -v 100 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 20 -b 20 >> Rr1R20.sts
p_test -v 1000 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 20 -b 20 >> Rr1R20.sts

# grid and trapezoid tests at resolution 100
p_test -v 3 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 100 -b 100 > Rr1R100.sts
p_test -v 4 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 100 -b 100 >> Rr1R100.sts
p_test -v 10 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 100 -b 100 >> Rr1R100.sts
p_test -v 20 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 100 -b 100 >> Rr1R100.sts
p_test -v 50 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 100 -b 100 >> Rr1R100.sts
p_test -v 100 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 100 -b 100 >> Rr1R100.sts
p_test -v 1000 -n 50 -i 50 -r 1 -p 0 -d -CGT -g 100 -b 100 >> Rr1R100.sts

# General polygons, but not sorting edges
make clean
export MAKEOPTS="-DTIMER -DRANDOM"
make
p_test -v 3 -n 50 -i 50 -r 0 -p 1 -d -PS > Rr0no_sort.sts
p_test -v 4 -n 50 -i 50 -r 0 -p 1 -d -PS >> Rr0no_sort.sts
p_test -v 10 -n 50 -i 50 -r 0 -p 1 -d -PS >> Rr0no_sort.sts
p_test -v 20 -n 50 -i 50 -r 0 -p 1 -d -PS >> Rr0no_sort.sts
p_test -v 50 -n 50 -i 50 -r 0 -p 1 -d -PS >> Rr0no_sort.sts
p_test -v 100 -n 50 -i 50 -r 0 -p 1 -d -PS >> Rr0no_sort.sts
p_test -v 1000 -n 50 -i 50 -r 0 -p 1 -d -PS >> Rr0no_sort.sts

p_test -v 3 -n 50 -i 50 -r 1 -p 0 -d -PS > Rr1no_sort.sts
p_test -v 4 -n 50 -i 50 -r 1 -p 0 -d -PS >> Rr1no_sort.sts
p_test -v 10 -n 50 -i 50 -r 1 -p 0 -d -PS >> Rr1no_sort.sts
p_test -v 20 -n 50 -i 50 -r 1 -p 0 -d -PS >> Rr1no_sort.sts
p_test -v 50 -n 50 -i 50 -r 1 -p 0 -d -PS >> Rr1no_sort.sts
p_test -v 100 -n 50 -i 50 -r 1 -p 0 -d -PS >> Rr1no_sort.sts
p_test -v 1000 -n 50 -i 50 -r 1 -p 0 -d -PS >> Rr1no_sort.sts


# Convex polygon tests, best algorithms
make clean
export MAKEOPTS="-DTIMER -DRANDOM -DSORT -DCONVEX -DHYBRID"
make
p_test -v 3 -n 50 -i 50 -r 1 -p 0 -d -EICMP > Rr1convex_hy.sts
p_test -v 4 -n 50 -i 50 -r 1 -p 0 -d -EICMP >> Rr1convex_hy.sts
p_test -v 10 -n 50 -i 50 -r 1 -p 0 -d -EICMP >> Rr1convex_hy.sts
p_test -v 20 -n 50 -i 50 -r 1 -p 0 -d -EICMP >> Rr1convex_hy.sts
p_test -v 50 -n 50 -i 50 -r 1 -p 0 -d -EICMP >> Rr1convex_hy.sts
p_test -v 100 -n 50 -i 50 -r 1 -p 0 -d -EICMP >> Rr1convex_hy.sts
p_test -v 1000 -n 50 -i 50 -r 1 -p 0 -d -EICMP >> Rr1convex_hy.sts

# random triangles (vs. regular triangles)
p_test -v 3 -n 50 -i 50 -r 0 -p 1 -d -EICMP > Rr0convex_hy.sts


# Convex polygon tests, best algorithms
make clean
export MAKEOPTS="-DTIMER -DRANDOM -DSORT -DCONVEX"
make
p_test -v 3 -n 50 -i 50 -r 1 -p 0 -d -CP > Rr1convex.sts
p_test -v 4 -n 50 -i 50 -r 1 -p 0 -d -CP >> Rr1convex.sts
p_test -v 10 -n 50 -i 50 -r 1 -p 0 -d -CP >> Rr1convex.sts
p_test -v 20 -n 50 -i 50 -r 1 -p 0 -d -CP >> Rr1convex.sts
p_test -v 50 -n 50 -i 50 -r 1 -p 0 -d -CP >> Rr1convex.sts
p_test -v 100 -n 50 -i 50 -r 1 -p 0 -d -CP >> Rr1convex.sts
p_test -v 1000 -n 50 -i 50 -r 1 -p 0 -d -CP >> Rr1convex.sts

# random triangles (vs. regular triangles)
p_test -v 3 -n 50 -i 50 -r 0 -p 1 -d -EICMP > Rr0convex.sts


# convex polygon tests, no randomizing, no sorting (slower)
make clean
export MAKEOPTS="-DTIMER -DCONVEX -DHYBRID"
make
p_test -v 3 -n 50 -i 50 -r 1 -p 0 -d -ECMP > Rr1convex_no_sort.sts
p_test -v 4 -n 50 -i 50 -r 1 -p 0 -d -ECMP >> Rr1convex_no_sort.sts
p_test -v 10 -n 50 -i 50 -r 1 -p 0 -d -ECMP >> Rr1convex_no_sort.sts
p_test -v 20 -n 50 -i 50 -r 1 -p 0 -d -ECMP >> Rr1convex_no_sort.sts
p_test -v 50 -n 50 -i 50 -r 1 -p 0 -d -ECMP >> Rr1convex_no_sort.sts
p_test -v 100 -n 50 -i 50 -r 1 -p 0 -d -ECMP >> Rr1convex_no_sort.sts
p_test -v 1000 -n 50 -i 50 -r 1 -p 0 -d -ECMP >> Rr1convex_no_sort.sts

# random triangles (vs. regular triangles)
p_test -v 3 -n 50 -i 50 -r 1 -p 0 -d -ECMP > Rr0convex_no_sort.sts


# Winding Number tests
make clean
export MAKEOPTS="-DTIMER -DRANDOM -DSORT -DWINDING"
make

# random polygons
p_test -v 3 -n 50 -i 50 -r 0 -p 1 -d -CW > Rr0winding.sts
p_test -v 4 -n 50 -i 50 -r 0 -p 1 -d -CW >> Rr0winding.sts
p_test -v 10 -n 50 -i 50 -r 0 -p 1 -d -CW >> Rr0winding.sts
p_test -v 20 -n 50 -i 50 -r 0 -p 1 -d -CW >> Rr0winding.sts
p_test -v 50 -n 50 -i 50 -r 0 -p 1 -d -CW >> Rr0winding.sts
p_test -v 100 -n 50 -i 50 -r 0 -p 1 -d -CW >> Rr0winding.sts
p_test -v 1000 -n 50 -i 50 -r 0 -p 1 -d -CW >> Rr0winding.sts

# regular polygons
p_test -v 3 -n 50 -i 50 -r 1 -p 0 -d -CW > Rr1winding.sts
p_test -v 4 -n 50 -i 50 -r 1 -p 0 -d -CW >> Rr1winding.sts
p_test -v 10 -n 50 -i 50 -r 1 -p 0 -d -CW >> Rr1winding.sts
p_test -v 20 -n 50 -i 50 -r 1 -p 0 -d -CW >> Rr1winding.sts
p_test -v 50 -n 50 -i 50 -r 1 -p 0 -d -CW >> Rr1winding.sts
p_test -v 100 -n 50 -i 50 -r 1 -p 0 -d -CW >> Rr1winding.sts
p_test -v 1000 -n 50 -i 50 -r 1 -p 0 -d -CW >> Rr1winding.sts

