;;; Spelling correction interface for GNU EMACS using "ispell"

;;; Walt Buehring
;;; Texas Instruments - Computer Science Center
;;; ARPA:  Buehring%TI-CSL@CSNet-Relay
;;; UUCP:  {smu, texsun, im4u, rice} ! ti-csl ! buehring

;;; Depends on the ispell program snarfed from MIT-PREP in early 
;;; 1986.  The only interactive command is "ispell-word" which should be
;;; bound to M-$.  If someone writes an "ispell-region" command, 
;;; I would appreciate a copy.

;;; To fully install this, add this file to your GNU lisp directory and 
;;; compile it with M-X byte-compile-file.  Then add the following to the
;;; appropriate init file:

;;;  (autoload 'ispell-word "ispell"
;;;    "Check the spelling of word in buffer." t)
;;;  (global-set-key "\e$" 'ispell-word)

;;; If run on a heavily loaded system, the timeout value in ispell-check 
;;; and the initial sleep time in ispell-init-process may need to be increased.

;;; No warranty expressed or implied.  All sales final.  Void where prohibited.
;;; If you don't like it, change it.

(defvar ispell-syntax-table nil)

(if (null ispell-syntax-table)
    ;; The following assumes that the standard-syntax-table
    ;; is static.  If you add words with funky characters
    ;; to your dictionary, the following may have to change.
    (progn
      (setq ispell-syntax-table (make-syntax-table))
      ;; Make certain characters word constituents
      (modify-syntax-entry ?' "w   " ispell-syntax-table)
      (modify-syntax-entry ?- "w   " ispell-syntax-table)
      ;; Get rid on existing word syntax on certain characters 
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
	      (re-search-backward "\\w" (dot-min) 'stay))
	  ;; Move to start of word
	  (re-search-backward "\\W" (dot-min) 'stay)
	  ;; Find start and end of word
	  (or (re-search-forward "\\w+" nil t)
	      (error "No word to check."))
	  (setq start (match-beginning 0)
		end (match-end 0)
		word (buffer-substring start end)))
      (set-syntax-table current-syntax))
    (or quietly (message "Checking spelling of %s..." (upcase word)))
    (setq poss (ispell-check word))
    (cond ((eq poss t)
	   (or quietly (message "Found %s" (upcase word))))
	  ((stringp poss)
	   (or quietly (message "Found it because of %s" (upcase poss))))
	  ((null poss)
	   (or quietly (message "Could Not Find %s" (upcase word))))
	  (t (setq replace (ispell-choose poss))
	     (if replace
		 (progn
		   (goto-char end)
		   (delete-region start end)
		   (insert-string replace)))))
    poss))


(defun ispell-choose (choices)
  "Display possible corrections from list CHOICES.  Return chosen word or nil 
if none chosen."
  (unwind-protect 
      (save-window-excursion
	(let ((count 0)
	      (words choices)
	      (pick -1)
	      (window-min-height 2))
	  (overlay-window 3)
	  (switch-to-buffer "*Choices*") (erase-buffer)
	  (setq mode-line-format "--  %b  --")
	  (while words
	    (if (> (+ 7 (current-column) (length (car words))) (window-width))
		(insert "\n"))
	    (insert "(" (+ count ?a) ") " (car words) "  ")
	    (setq words (cdr words)
		  count (1+ count)))
	  (select-window (next-window))
	  (while (eq pick -1)
	    (message "Enter letter to replace word;  Space to flush")
	    (let* ((char (read-char))
		   (num (1+ (- (upcase char) ?A))))
	      (cond ((= char ? ) (setq pick 0))
		    ((or (<= num 0) (> num count)) (ding))
		    (t (setq pick num)))))
	  (and (> pick 0) (nth (1- pick) choices))))
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

;;; create signal used by ispell-filter and ispell-check
(put 'ispell-output 'error-conditions '(ispell-output))

(defun ispell-check (word)
"Check spelling of string WORD, return either t for an exact match, a string
containing the root word for a match via suffix removal, a list of possible 
correct spellings, or nil for a complete miss."
  (ispell-init-process)
  (send-string ispell-process (concat word "\n"))
  (condition-case output
      (progn
	(sleep-for 20)
	(error "Timeout waiting for ispell process output"))
    (ispell-output (ispell-parse-output (car (cdr output))))))

(defun ispell-parse-output (output)
"Parse the OUTPUT string of 'ispell' and return a value as specified by the 
'ispell-check' function."
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


(defvar ispell-process-output ""
  "Holds partial output from the 'ispell' process")

(defun ispell-filter (process output)
  "The filter-function for 'ispell'.  Signals complete line using the 
ispell-output signal"
  (if (string= "\n" (substring output (1- (length output))))
      (progn
	(setq output (concat ispell-process-output
			     (substring output 0 (1- (length output))))
	      ispell-process-output "")
	(signal 'ispell-output (list output)))
      (setq ispell-process-output (concat ispell-process-output output))))

(defun ispell-init-process ()
  "Check status of 'ispell' process and start if necessary; set up 
filter function for output."
  (if (or (not ispell-process)
	  (not (eq (process-status ispell-process) 'run)))
      (progn
	(message "Starting new ispell process...")
	(and (get-buffer "*ispell*") (kill-buffer "*ispell*"))
	(setq ispell-process (start-process "ispell" "*ispell*"
					   "ispell" "-a"))
	(set-process-filter ispell-process 'ispell-filter)
	(process-kill-without-query ispell-process)
	(sit-for 3))))

