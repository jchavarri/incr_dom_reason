open! Core_kernel;
open! Incr_dom;
open! Js_of_ocaml;

let () =
  Start_app.simple(
    (module App),
    ~initial_model=
      App.Model.Fields.create(~counters=Int.Map.singleton(0, 13)),
  );
