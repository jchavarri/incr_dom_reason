open! Core_kernel;
open Incr_dom;

module Model = {
  [@deriving (sexp, fields, compare)]
  type t = {counters: Int.Map.t(int)};

  let add_new = t => {
    let counters =
      Int.Map.set(t.counters, ~key=Map.length(t.counters), ~data=0);
    {counters: counters};
  };

  /* no bounds checks */
  let update = (t, pos, diff) => {
    let old_val = Map.find_exn(t.counters, pos);
    let counters = Int.Map.set(t.counters, ~key=pos, ~data=old_val + diff);
    {counters: counters};
  };

  let cutoff = (t1, t2) => compare(t1, t2) == 0;
};

module Action = {
  [@deriving sexp]
  type t =
    | New_counter
    | Update(int, int); /* pos, diff */

  let should_log = _ => true;
};

module State = {
  type t = unit;
};

let apply_action = (action, model, _state) =>
  switch ((action: Action.t)) {
  | New_counter => Model.add_new(model)
  | Update(pos, diff) => Model.update(model, pos, diff)
  };

let update_visibility = m => m;

let on_startup = (~schedule as _, _) => Async_kernel.return();

let on_display = (~old as _, _, _) => ();

let view = (m: Incr.t(Model.t), ~inject) => {
  open Incr.Let_syntax;
  open Vdom;
  let on_add_new_click = Attr.on_click(_ev => inject(Action.New_counter));
  let add_new_counter_button =
    Node.div(
      [],
      [Node.button([on_add_new_click], [Node.text("add new counter")])],
    );

  let button = (txt, pos, diff) => {
    let on_click = _ev => inject(Action.Update(pos, diff));
    Vdom.Node.button([Attr.on_click(on_click)], [Node.text(txt)]);
  };

  let%map elements =
    Incr.Map.mapi'(
      m >>| Model.counters,
      ~f=(~key as pos, ~data as value) => {
        let button_minus = button("-", pos, -1);
        let button_plus = button("+", pos, 1);
        let%map value = value;
        Node.div(
          [],
          [button_minus, Node.text(Int.to_string(value)), button_plus],
        );
      },
    );

  Node.body([], [add_new_counter_button, ...Map.data(elements)]);
};
