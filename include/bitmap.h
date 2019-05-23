#ifndef BITMAP_H
#define BITMAP_H

struct bitmap * bitmap_create( int w, int h );
void            bitmap_delete( struct bitmap *b );
struct bitmap * bitmap_load( const char *file );
int             bitmap_save( struct bitmap *b, const char *file );

int   bitmap_get( struct bitmap *b, int x, int y );
void  bitmap_set( struct bitmap *b, int x, int y, int value );
int   bitmap_width( struct bitmap *b );
int   bitmap_height( struct bitmap *b );
void  bitmap_reset( struct bitmap *b, int value );
int  *bitmap_data( struct bitmap *b );
