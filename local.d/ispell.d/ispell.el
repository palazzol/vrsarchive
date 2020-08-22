;;; Spelling correction interface for GNU EMACS using "ispell"

;;; Walt Buehring
;;; Texas Instruments - Computer Science Center
;;; ARPA:  Buehring%TI-CSL@CSNet-Relay
;;; UUCP:  {smu, texsun, im4u, rice} ! ti-csl ! buehring

;;; ispell-region and associated routines added by
;;; Perry Smith
;;; pedz@bobkat
;;; Tue Jan 13 20:18:02 CST 1987

;;; extensively modified by Mark Davies and Andrew Vignaux
;;; {mark,andrew}@vuwcomp
;;; Sun May 10 11:45:04 NZST 1987

;;; Depends on the ispell program snarfed from MIT-PREP in early 
;;; 1986.  The only interactive command is "ispell-word" which should be
;;; bound to M-$.  If someone writes an "ispell-region" command, 
;;; I would appreciate a copy. -- ispell-region, ispell-buffer now added.

;;; To fully install this, add this file to your GNU lisp directory and 
;;; compile it with M-X byte-compile-file.  Then add the following to the
;;; appropriate init file:

;;;  (autoload 'ispell-word "ispell"
;;;    "Check the spelling of word in buffer." t)
;;;  (global-set-key "\e$" 'ispell-word)
;;;  (autoload 'ispell-region "ispell"
;;;    "Check the spelling of region." t)
;;;  (autoload 'ispell-buffer "ispell"
;;;    "Check the spelling of buffer." t)

;;; If run on a heavily loaded system, the initial sleep time in
;;; ispell-init-process may need to be increased.

;;; No warranty expressed or implied.  All sales final.  Void where prohibited.
;;; If you don't like it, change it.

(defconst ispell-out-name " *ispell*"
  "Name of the buffer that is associated with the 'ispell' process")

(defconst ispell-temp-name " *ispell-temp*"
  "Name of the temporary buffer that 'ispell-region' uses to hold the
filtered region")

(defvar ispell-program-name "ispell"
  "Program invoked by ispell-word and ispell-region commands.")

(defvar ispell-syntax-table nil)

(if (null ispell-syntax-table)
    ;; The following assumes that the standard-syntax-table
    ;; is static.  If you add words with funky characters
    ;; to your dictionary, the following may have to change.
    (progn
      (setq ispell-syntax-table (make-syntax-table))
      ;; Make certain characters word constituents
      ;; (modify-syntax-entry ?' "w   " ispell-syntax-table)
      ;; (modify-syntax-entry ?- "w   " ispell-syntax-table)
      ;; Get rid on existing word syntax on certain characters 
      (modify-syntax-entry ?0 ".   " ispell-syntax-table)
      (modify-syntax-entry ?1 ".   " ispell-syntax-table)
      (modify-syntax-entry ?2 ".   " ispell-syntax-table)
      (modify-syntax-entry ?3 ".   " ispell-syntax-table)
      (modify-syntax-entry ?4 ".   " ispell-syntax-table)
      (modify-syntax-entry ?5 ".   " ispell-syntax-table)
      (modify-syntax-entry ?6 ".   " ispell-syntax-table)
      (modify-syntax-entry ?7 ".   " ispell-syntax-table)
      (modify-syntax-entry ?8 ".   " ispell-syntax-table)
      (modify-syntax-entry ?9 ".   " ispell-syntax-table)
      (modify-syntax-entry ?$ ".   " ispell-syntax-table)
      (modify-syntax-entry ?% ".   " ispell-syntax-table)))


(defun ispell-word (&optional quietly)
  "Check spelling of word at or before dot.
If word not found in dictionary, display possible corrections in a window 
and let user select."
  (interactive)
  (let* ((current-syntax (syntax-table))
	 start end word poss replace)
    (unwind-protect
	(save-excursion
	  ;; Ensure syntax table is reasonable 
	  (set-syntax-table ispell-syntax-table)
	  ;; Move backward for word if not already on one.
	  (if (not (looking-at "\\w"))
	      (re-search-backward "\\w" (point-min) 'stay))
	  ;; Move to start of word
	  (re-search-backward "\\W" (point-min) 'stay)
	  ;; Find start and end of word
	  (or (re-search-forward "\\w+" nil t)
	      (error "No word to check."))
	  (setq start (match-beginning 0)
		end (match-end 0)
		word (buffer-substring start end)))
      (set-syntax-table current-syntax))
    (ispell-init-process)		; erases ispell output buffer
    (or quietly (message "Checking spelling of %s..." (upcase word)))
    (save-excursion
      (set-buffer ispell-out-name)
      (send-string ispell-process (concat word "\n"))
      ;; wait until we have a complete line
      (while (progn
	       (goto-char (point-max))
	       (/= (preceding-char) ?\n))
	(accept-process-output ispell-process))
      (goto-char (point-min))
      (setq poss (ispell-parse-output
		  (buffer-substring (point) 
				    (progn (end-of-line) (point))))))
    (cond ((eq poss t)
	   (or quietly (message "Found %s" (upcase word))))
	  ((stringp poss)
	   (or quietly (message "Found it because of %s" (upcase poss))))
	  ((null poss)
	   (or quietly (message "Could Not Find %s" (upcase word))))
	  (t (setq replace (ispell-choose poss word))
	     (if replace
		 (progn
		    (goto-char end)
		    (delete-region start end)
		    (insert-string replace)))))
    poss))


(defun ispell-choose (choices word)
  "Display possible corrections from list CHOICES.  Return chosen word
if one is chosen; Return nil to keep word"
  (unwind-protect 
      (save-window-excursion
	(let ((count 0)
	      (line 2)
	      (words choices)
	      (window-min-height 2)
	      char num result)
	  (save-excursion
	    (set-buffer (get-buffer-create "*Choices*")) (erase-buffer)
	    (setq mode-line-format "--  %b  --")
	    (while words
	      (if (<= (+ 7 (current-column) (length (car words)))
		      (window-width))
		  nil
		(insert "\n")
		(setq line (1+ line)))
	      (insert "(" (+ count ?0) ") " (car words) "  ")
	      (setq words (cdr words)
		    count (1+ count))))
	  (overlay-window line)
	  (switch-to-buffer "*Choices*")
	  (select-window (next-window))
	  (while (eq t
		     (setq result
			   (progn
			     (message "Enter letter to replace word;  Space to leave unchanged")
			     (setq char (read-char))
			     (setq num (- char ?0))
			     (cond ((= char ? ) nil)
				   ((= char ?i)
				    (send-string ispell-process
						 (concat "*" word "\n"))
				    (if (get-buffer ispell-temp-name)
					(save-excursion
					  (set-buffer ispell-temp-name)
					  (save-excursion
					    (replace-regexp (concat "^" word "$")
							    (concat "+" word)))))
				    nil)
				   ((= char ?a)
				    (send-string ispell-process
						(concat "@" word "\n"))
				    (if (get-buffer ispell-temp-name)
					(save-excursion
					  (set-buffer ispell-temp-name)
					  (save-excursion
					    (replace-regexp (concat "^" word "$")
							    (concat "+" word)))))
				    nil)
				   ((= char ?r) (read-string "Replacement: " nil))
				   ((and (>= num 0) (< num count)) (nth num choices))
				   ((= char ?\C-r)
					; Note: does not reset syntax table
				    (save-excursion (recursive-edit)) t) ; dangerous
				   ((= char ?\C-z)
				    (suspend-emacs) t)
				   ((= char help-char)
				    (message "[r]eplace; [a]ccept for session; [i] accept and add to private dictionary")
				    (sit-for 3) t)
				   (t (ding) t))))))
	  result))
    ;; Protected forms...
    (bury-buffer "*Choices*")))


(defun overlay-window (height)
  "Create a (usually small) window with HEIGHT lines and avoid
recentering."
  (save-excursion
    (let ((oldot (save-excursion (beginning-of-line) (dot)))
	  (top (save-excursion (move-to-window-line height) (dot)))
	  newin)
      (if (< oldot top) (setq top oldot))
      (setq newin (split-window-vertically height))
      (set-window-start newin top))))


(defvar ispell-process nil
  "Holds the process object for 'ispell'")

(defun ispell-parse-output (output)
"Parse the OUTPUT string of 'ispell' and return either t for an exact
match, a string containing the root word for a match via suffix
removal, a list of possible correct spellings, or nil for a complete
miss."
  (cond
   ((string= output "*") t)
   ((string= output "#") nil)
   ((string= (substring output 0 1) "+")
    (substring output 2))
   (t
    (let ((choice-list '()))
      (while (not (string= output ""))
	(let* ((start (string-match "[A-z]" output))
	       (end (string-match " \\|$" output start)))
	  (if start
	      (setq choice-list (cons (substring output start end)
				      choice-list)))
	  (setq output (substring output (1+ end)))))
      choice-list))))


(defun ispell-init-process ()
  "Check status of 'ispell' process and start if necessary."
  (if (and ispell-process
	   (eq (process-status ispell-process) 'run))
      (save-excursion
	(set-buffer ispell-out-name)
	(erase-buffer))
    (message "Starting new ispell process...")
    (and (get-buffer ispell-out-name) (kill-buffer ispell-out-name))
    (setq ispell-process (start-process "ispell" ispell-out-name
					ispell-program-name "-A"))
    (process-kill-without-query ispell-process)
    (sit-for 3)))

(defvar ispell-filter-hook "/usr/bin/tr"
  "Filter to pass a region through before sending it to ispell.
Must produce output one word per line.  Typically this is set to tr,
deroff, detex, etc.")
(make-variable-buffer-local 'ispell-filter-hook)

(defvar ispell-filter-hook-args '("-cs" "A-Za-z" "\n")
  "Argument LIST to pass to ispell-filter-hook")
(make-variable-buffer-local 'ispell-filter-hook-args)

; This routine has certain limitations brought about by the filter
; hook.  For example, deroff will take ``\fBcat\fR'' and spit out
; ``cat''.  This is hard to search for since word-search-forward will
; not match at all and search-forward for ``cat'' will match
; ``concatenate'' if it happens to occur before.
; `ispell-region' filters the region into `*ispell-temp*', writes the
; buffer to a temporary file, and sends a ``&Include_File&foobar''
; string to the ispell process which is writing into `*ispell*'.
; `ispell-region' then searches `*ispell*' for a spelling error (`#' or
; `&'), checks the `*ispell-temp*' buffer for the misspelled word and
; then skips forward `count' words (the number of correct lines in
; `*ispell*') in the region.  It then searches for the misspelled
; word.  This is not a foolproof heuristic but it is fast and works
; most of the time.

(defun ispell-region (start end)
  "Check a region for spelling errors interactively.  The variable
which should be buffer or mode specific ispell-filter-hook is called
to filter out text processing commands."
  (interactive "r")
  (let ((this-buf (current-buffer))
	(spell-file (make-temp-name "/usr/tmp/is"))
	(spell-buf (get-buffer-create ispell-temp-name))
	(current-syntax (syntax-table))
	(tracker 1)
	word poss replace endbound ispell-out)
    (ispell-init-process)
    (setq ispell-out (get-buffer ispell-out-name))
    (unwind-protect
	(save-excursion
	  (message "Prefrobnicating...")
	  (sit-for 0)
	  (set-syntax-table ispell-syntax-table)
	  (set-buffer spell-buf)
	  (erase-buffer)
	  (set-buffer this-buf)
	  (apply 'call-process-region 
		 (append (list start end ispell-filter-hook nil spell-buf nil)
			 ispell-filter-hook-args))
	  (goto-char start)
	  (set-buffer spell-buf)
	  (and (/= (preceding-char) ?\n) ; couple of hacks for tr
	       (insert "\n"))
	  (goto-char (point-min))
	  (while (= (following-char) ?\n)
	    (delete-char 1))
	  (write-region (point-min) (point-max) spell-file nil 1)
	  (send-string ispell-process 
		       (concat "&Include_File&" spell-file "\n"))
	  (message "Looking for a misspelled word") (sit-for 0)
	  (while (not (eobp))
	    (set-buffer ispell-out)
	    (goto-char (point-max))
	    (beginning-of-line)
	    (setq endbound (point))
	    (goto-char tracker)
	    (if (prog1
		    (not (re-search-forward "^[#&]" endbound 1))
		  (beginning-of-line)
		  (setq count (count-lines tracker (point))
			tracker (point))
		  (set-buffer spell-buf)
		  (forward-line count))
		(accept-process-output)	; give it some time to get something
	      (setq word (buffer-substring (point)
					   (progn (end-of-line) (point))))
	      (forward-char 1)
	      (set-buffer ispell-out) ; (goto-char tracker)
	      (setq poss (ispell-parse-output
			  (buffer-substring (point) 
					    (progn (end-of-line) (point)))))
	      (forward-char 1)
	      (setq tracker (point))
	      (set-buffer this-buf)
	      (re-search-forward "\\W*\\(\\w+\\)" nil t (1- count)) ; get close
	      (if (string= "+" (substring word 0 1))
		  (search-forward (substring word 1) nil t)
		(if (search-forward word nil t)
		    (progn
		      (sit-for 0)
		      (setq replace (ispell-choose poss word))
		      (if replace
			  (replace-match replace)))
		  (message "Can not find %s in original text -- Any key to continue" word)
		  (and (= ?\C-z (read-char)) (suspend-emacs)))
		(message "Looking for a misspelled word") (sit-for 0))
	      (set-buffer spell-buf))))
      (message "")
      (set-syntax-table current-syntax)
      (and (file-exists-p spell-file)
	   (delete-file spell-file)))))

(defun ispell-buffer () 
  "Check the current buffer for spelling errors interactively.  The variable
which should be buffer or mode specific ispell-filter-hook is called to
filter out text processing commands."
  (interactive)
  (ispell-region (point-min) (point-max)))
