#include "m_pd.h"
#include "../dependencies/jl.cpp.lib/control/gesture/IntervalMap.h"

class PdIntervalMap;

static t_class *map_class;

//----------------------------------------------------------------------------//

typedef struct _map {
  t_object x_obj;
  t_float lastValueIn;

  PdIntervalMap *map;
  t_outlet *f_out;
} t_map;

//----------------------------------------------------------------------------//

class PdIntervalMap : public jl::IntervalMap<float> {
private:
  t_map *x;

public:
  PdIntervalMap() : IntervalMap<float>() {}
  virtual ~PdIntervalMap() {}

  void setObject(t_map *obj) {
    x = obj;
  }
};

//----------------------------------------------------------------------------//

void map_float(t_map *x, t_floatarg f) {
  x->lastValueIn = f;
  outlet_float(x->f_out, x->map->process(x->lastValueIn));
}

void map_bang(t_map *x) {
  outlet_float(x->f_out, x->map->process(x->lastValueIn));
}

void map_inputmin(t_map *x, t_floatarg f) {
  x->map->setInputMin(f);
}

void map_inputmax(t_map *x, t_floatarg f) {
  x->map->setInputMax(f);
}

void map_outputmin(t_map *x, t_floatarg f) {
  x->map->setOutputMin(f);
}

void map_outputmax(t_map *x, t_floatarg f) {
  x->map->setOutputMax(f);
}

void map_xfactor(t_map *x, t_floatarg f) {
  x->map->setXFactor(f);
}

void map_sfactor(t_map *x, t_floatarg f) {
  x->map->setSFactor(JL_MAX(f, 0));
}

//----------------------------------------------------------------------------//

void *map_new(t_symbol *s, int argc, t_atom *argv) {
  t_map *x = (t_map *)pd_new(map_class);

  x->lastValueIn = 0;

  x->map = new PdIntervalMap();
  x->map->setObject(x);

  switch (argc) {
    case 0:
      break;
    case 1:
      x->map->setInputMax(atom_getfloat(argv));
      break;
    case 2:
      x->map->setInputMin(atom_getfloat(argv));
      x->map->setInputMax(atom_getfloat(argv + 1));
      break;
    case 3:
      x->map->setInputMin(atom_getfloat(argv));
      x->map->setInputMax(atom_getfloat(argv + 1));
      x->map->setOutputMax(atom_getfloat(argv + 2));
      break;

    case 6:
      x->map->setSFactor(atom_getfloat(argv + 5));
    case 5:
      x->map->setXFactor(atom_getfloat(argv + 4));
    case 4:
      x->map->setInputMin(atom_getfloat(argv));
      x->map->setInputMax(atom_getfloat(argv + 1));
      x->map->setOutputMin(atom_getfloat(argv + 2));
      x->map->setOutputMax(atom_getfloat(argv + 3));
      break;
  }

  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("inputmin"));
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("inputmax"));
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("outputmin"));
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("outputmax"));
  // inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("xfactor"));
  // inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("sfactor"));

  x->f_out = outlet_new(&x->x_obj, &s_float);

  return (void *)x;
}

extern "C" {

void map_setup(void) {
  map_class = class_new(gensym("map"),
                        (t_newmethod)map_new,
                        0,
                        sizeof(t_map),
                        CLASS_DEFAULT,
                        A_GIMME,
                        0);

  class_addbang(map_class, map_bang);
  class_addfloat(map_class, map_float);
  class_addmethod(map_class, (t_method)map_inputmin, gensym("inputmin"), A_DEFFLOAT, 0);
  class_addmethod(map_class, (t_method)map_inputmax, gensym("inputmax"), A_DEFFLOAT, 0);
  class_addmethod(map_class, (t_method)map_outputmin, gensym("outputmin"), A_DEFFLOAT, 0);
  class_addmethod(map_class, (t_method)map_outputmax, gensym("outputmax"), A_DEFFLOAT, 0);
  class_addmethod(map_class, (t_method)map_xfactor, gensym("xfactor"), A_DEFFLOAT, 0);
  class_addmethod(map_class, (t_method)map_sfactor, gensym("sfactor"), A_DEFFLOAT, 0);
}

}; /* end extern "C" */
