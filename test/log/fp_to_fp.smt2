(set-logic QF_FP)
(define-fun x () Float16 (_ +zero 5 11))
(define-fun y () Float16 (_ +zero 5 11))
(declare-const a Float16);
(assert (= x ((_ to_fp 5 11) (_ bv0 16))))
(assert (= x ((_ to_fp 5 11) RNE x)))
(assert (= x ((_ to_fp 5 11) RNE a)))
(assert (= x ((_ to_fp 5 11) RNE (_ bv0 16))))
(check-sat)
