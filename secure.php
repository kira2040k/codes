<?php

// This code will prevent cross site scripting(XSS) on your website 99%
// Just include it on php files you have
session_start();
foreach ($_GET as $par => $value){
    $_GET[$par] = htmlspecialchars($value);
   } 
   foreach ($_POST as $par => $value){
    $_POST[$par] = htmlspecialchars($value);
   } 
   foreach ($_SESSION as $name => $value)
{
    $_SESSION[$name] = htmlspecialchars($value);
}
?>
