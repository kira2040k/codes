<?php

foreach ($_GET as $par => $value){
    $_GET[$par] = htmlspecialchars($value);
    
   } 

?>
