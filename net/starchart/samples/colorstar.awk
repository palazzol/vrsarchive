{t = "  "}
(substr($0,15,1) == "C") {t = "y9"}
(substr($0,15,1) == "N") {t = "g9"}
(substr($0,15,1) == "G") {t = "r9"}
{printf "%s%s%s\n", substr($0,1,16), t, substr($0,19)}

