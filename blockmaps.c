#include <r_core.h>
#include <r_io.h>
#include <r_util.h>
#include <allegro.h>
#include <unistd.h>

#if 0
int use_these_colors[9] = {
	makecol (0xff, 0x00, 0x00),
	makecol (0x00, 0xff, 0x00),
	makecol (0x00, 0x00, 0xff),
	makecol (0xff, 0x14, 0x93),
	makecol (0xff, 0xb9, 0x0f),
	makecol (0x97, 0xff, 0xff),
	makecol (0xff, 0xff, 0x00),
	makecol (0xff, 0x00, 0xff),
	makecol (0x00, 0xff, 0xff)
};
#endif

static int use_these_colors[9] = {
	0xff0000,
	0x00ff00,
	0x0000ff,
	0xff1493,
	0xffb90f,
	0x97ffff,
	0xffff00,
	0xff00ff,
	0x00ffff,
};

void entry(RCore *core)
{
	BITMAP *canvas;
	PALETTE pal;
	SdbListIter *iter;
	RIOMap *map;
	RQueue *relevant_maps;
	ut64 from, to;
	int elements = 1, height, x, xend, y;
	char filename[128];
	r_io_map_cleanup (core->io);
	if (!core->io->va || !core->io->maps || !(relevant_maps = r_queue_new (4))) {
		return;
	}
	from = core->offset;
	to = core->offset + core->blocksize - 1;
	ls_foreach_prev (core->io->maps, iter, map) {
		if (((from <= map->from) && (map->from <= to)) ||
			((from <= map->to) && (map->to <= to)) ||
			((from > map->from) && (map->to > to))) {
			r_queue_enqueue (relevant_maps, map);
			elements++;
		}
	}
	y = height = 480 / elements;
	allegro_init();
	set_color_depth( 24 );
	get_palette (pal);
	canvas = create_bitmap (640, 480);
	clear_to_color (canvas, makecol (0,0,0));
	while (!r_queue_is_empty (relevant_maps)) {
		map = (RIOMap *)r_queue_dequeue (relevant_maps);
		elements = elements % 9;
		if (map->from < from) {
			x = 0;
		} else {
			x = (int)(map->from - from) * (640 / core->blocksize);
		}
		if (to < map->to) {
			xend = 639;
		} else {
			xend = (map->to - from) * (640 / core->blocksize);
		}
		rectfill (canvas, x, y, xend, y+height, use_these_colors[elements]);
		textprintf_ex (canvas, font, x + ((xend - x)/2), y + (height/2),
				use_these_colors[elements] ^ 0xffffff, use_these_colors[elements], "%d", map->id);
		elements++;
		y += height;
	}
	r_queue_free (relevant_maps);
	snprintf (filename, 128, "maps.%"PFMT64x".bmp", r_sys_now());
	save_bmp (filename, canvas, pal);
	destroy_bitmap( canvas );
//	return 0;
} /* end function main */
END_OF_MAIN()


