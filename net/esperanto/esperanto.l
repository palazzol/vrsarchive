; Do a simple word-for-word translation of Esperanto into English
; Neal McBurnett, 9/84, ihnp4!druny!neal
; Usage:
; 	lisp
;	(load 'esperanto)
;	(readdict) 			;takes a few minutes....

;	(xlate 'amiko)			;translate one word

;	(interpret)			;continuous translation
;	saluton, KARLO.  kiel vi sanas?
;	tre bone, dankon.  kio estas tio?
;	^d		;the terminator (or 'Q')

;	(setio 'sample)	;input file will be sample.esp, output will go to sample.trans
;	(interpret)

; Only lower case words are recognized.  Proper names should appear in CAPITALS.

; Obvious improvements:
;	treat accented letters (^c, etc) as one letter when cdring down list
;	add categories to the dictionary to avoid conflicts like 'kaj' (and)
;	   vs 'kaj/o' (quay):
;		roots (must have grammatical ending)
;		suffixes (also must have grammatical ending)
;		grammatical endings (only 'o' allowed in middle of word)
;		other (preps, conjunctions, pronouns, etc): ending optional
;	try to quickly identify common cases, e.g. <root><grammitical ending>


; xlate works by repeatedly peeling off the largest prefix or suffix of the word which
;  it can find until nothing is left, or it can't peel anything else off.
;  Sometimes this doesn't work, e.g. 'sendependa' = sen-depend-a, not send-e-pend-a.
; bug: purigado: tries to figure 'ig' as prefix, finds 'i'....
; tests: mi kion samtempo samtempe mal o  kantodoj

(defun xlate (word)
	(setq word (explode word))
	(setq root nil)
	(setq suffixes nil)
	(setq prefixes nil)

	(checkroot)
	(loop while (let ((result (checksuffix nil word)))
			 (or (checkprefix nil (reverse word)) result) ))
	(checkroot)
	
	(cond (word
		(concatl (append (reverse prefixes) (append '(|(|) word '(|)|) ) suffixes)) )
	      (t
		(concatl (append (reverse prefixes) root suffixes)) )
	)
)

(defun checkroot ()
	(cond (word 
	       (setq root (get (implode word) 'esp-root))
	       (cond (root	(setq root (list root))  (setq word nil)))
	      )
	)
)

(defun checksuffix (notw w)
	(prog ()
	loop
		(cond	((null w)	(return nil))
			((looksuffix w)	(setq word (reverse notw)) (return w))
			(t		(push (pop w) notw)	(go loop)) ) ) )

(defun checkprefix (notw w)
	(prog ()
	loop
		(cond	((null w)	(return nil))
			((lookprefix w)	(setq word notw) (return w))
			(t		(push (pop w) notw)	(go loop)) ) ) )

(defun looksuffix (fix)
	(cond	((or (setq prop (get (implode fix) 'esp-sfix))
		     ; rely on checkprefix to find a root in the original word
		     (and suffixes
		     	(setq prop (get (implode fix) 'esp-root)) ) )
		 (setq suffixes (cons prop suffixes))
		)) )

(defun lookprefix (fix)
	(setq fix (reverse fix))
	(cond	((or (setq prop (get (implode fix) 'esp-pfix))
		     (setq prop (get (implode fix) 'esp-root)) )
		 (setq prefixes (cons prop prefixes))
		)) )


; continuous interpretation
(defun interpret ()
	(setq readtable espreadtable)
	(loop while (not (eq (setq input (read inport 'Q)) 'Q))
	  do
		; "=newword def" will define newword in the nrootlist
		(cond	((eq input '=)	(setq newword (read inport))
					(read inport)	;skip space
					(setq def (read inport))
					(putprop newword def 'esp-root)
					(setq nrootlist (cons newword
							(cons def nrootlist)) ) )
			((member input (explode punct))	(patom input outport))
			(t		(patom (xlate input) outport))
		)
	)
	(drain outport)
	(setq readtable dreadtable)
)


; auxiliary routines

; read the dictionary in
(defun readdict ()
	(setq nrootlist (readfile 'esp.nroot))
	(loadprops nrootlist 'esp-root)

	(setq suflist (readfile 'esp.suf))
	(loadprops suflist 'esp-sfix)

	(setq preflist (readfile 'esp.pref))
	(loadprops preflist 'esp-pfix)

	(setq rootlist (readfile 'esp.root))
	(loadprops rootlist 'esp-root)
)

(defun setio (file)
	(setq inport (infile (concat file '.esp)))
	(setq outport (outfile (concat file '.trans))) )

(defun readfile (file)
	(setq fd (infile file))
	(setq list (read fd))
	(close fd)
	list
)

(defun writefile (l file)
	(setq fd (outfile file))
	(print l fd)
	(print '|
| fd)
	(close fd)
	l
)

(defun saveroots ()
	(length (writefile nrootlist 'esp.nroot))
)

(defun loadprops (l prop)
	(loop while l
		(putprop (car l) (cadr l) prop)
		(setq l (cddr l)) ) )

(defun resetreadtable ()
	(setq readtable dreadtable) )

; don't do these separately: have one read get them all
(progn	(setq punct '|	 !@#$%&*();:"'=-`~.,<>/?
|)

	(setq espreadtable (makereadtable))
	(setq dreadtable readtable)
	(setq readtable espreadtable)
	(mapcar '(lambda (c) (setsyntax c 'vsingle-character-symbol))
		(explode punct) )
	(setsyntax '\\ 'vmacro 'resetreadtable)
	(setq readtable dreadtable) )

(setq inport piport)
(setq outport poport)
