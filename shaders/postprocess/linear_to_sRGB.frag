void linear_to_sRGB()
{
    float samples[ 3 ];
    samples[ 0 ] = fragment_out_color.r;
    samples[ 1 ] = fragment_out_color.g;
    samples[ 2 ] = fragment_out_color.b;
    
    for( int i = 0; i < 3; ++i )
        if( samples[ i ] <= 0.0031308 )
            samples[ i ] *= 12.92;
        else
            samples[ i ] = 1.055 * pow( samples[ i ], 1 / 2.4 );
    
    fragment_out_color.r = samples[ 0 ];
    fragment_out_color.g = samples[ 1 ];
    fragment_out_color.b = samples[ 2 ];
}
