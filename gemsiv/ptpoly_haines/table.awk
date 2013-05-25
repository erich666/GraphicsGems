# Run results from statrun.tst through this script, e.g.
#    table.awk *.stats
# to generate tables of timings

for dog in $*
do
cat $dog | awk '
BEGIN {
    count = -1
}
{
    if ( $1 == "Polygons" ) {
	count++
	if ( !count ) {
	    title = $0
	    title_on = 1
	}
	if ( $4 == "to" ) {
	    nv[count] = $3"-"$5
	} else {
	    nv[count] = $3
	}
    } else if ( title_on ) {
	if ( !count ) {
	    if ( $1 == "Testing" ) {
		title_on = 0
	    } else {
		title = title "\n" $0
	    }
	}
    } else if ( $3 == "time:" ) {
	t[$1,count] = $4
	found[$1] = 1
    } else if ( $2 == "%" ) {
	pc[count] = $1
    }
}
END {
    print title

    printf( "\n\t\t   Number of vertices\n\t" )
    for ( i = 0 ; i <= count ; i++ ) {
	printf( "\t    %s",nv[i] )
    }
    printf( "\n\n" ) ;

    for ( name in found ) {
	printf( "%s\t", name )
	if ( length( name ) < 8 ) {
	    printf( "\t" )
	}
	for ( i = 0 ; i <= count ; i++ ) {
	    printf( "%8.1f",t[name,i] )
	}
	printf( "\n" ) ;
    }

    printf( "\ninside %\t" )
    for ( i = 0 ; i <= count ; i++ ) {
	printf( "%8.1f",pc[i] )
    }
    printf( "\n\n\n" ) ;
}'
done
