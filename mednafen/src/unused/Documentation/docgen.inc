<?php

$mednafen_version = "0.9.15-WIP";

$curmod = "";
$cursection = "";

$ps_string = "";
$toc_string = "";
$settings = array();
$section_depth = 0;

function GetMTime()
{
 $sf_mt = filemtime($_SERVER["SCRIPT_FILENAME"]);
 $sd_mt = filemtime("settings.def");
 $dg_mt = filemtime("docgen.inc");

 $mt = $sf_mt;

 if($sd_mt > $mt)
  $mt = $sd_mt;

 if($dg_mt > $mt)
  $mt = $dg_mt;

 return($mt);
}

function ReadSettings()
{
 global $settings;

 $settings = array();

 $fp = fopen("settings.def", "rb");

 while(!feof($fp))
 {
  $setting = array();

  $setting['name'] = trim(fgets($fp));
  $setting['flags'] = trim(fgets($fp));
  $setting['description'] = stripcslashes(trim(fgets($fp)));
  $setting['description_extra'] = stripcslashes(trim(fgets($fp)));
  $setting['type'] = trim(fgets($fp));
  $setting['default_value'] = trim(fgets($fp));
  $setting['minimum'] = trim(fgets($fp));
  $setting['maximum'] = trim(fgets($fp));

  $enum_count = (int)fgets($fp);
  $enum_list = array();

  while($enum_count--)
  {
   $ele = array();

   $ele['string'] = trim(fgets($fp));
   $ele['description'] = stripcslashes(trim(fgets($fp)));
   $ele['description_extra'] = stripcslashes(trim(fgets($fp)));

   array_push($enum_list, $ele);
  }

  $setting['enum_list'] = $enum_list;

  array_push($settings, $setting);
 }


 fclose($fp);
}


function BeginPage($module_name, $module_description)
{
 global $mednafen_version, $curmod, $cursection, $ps_string, $toc_string, $section_depth;

 ReadSettings();

 $curmod = $module_name;

 $section_depth++;

 $ps_string = '
<html>
 <head>
  <title>Mednafen ' . htmlspecialchars($module_description) . ' Documentation</title>
  <link rel="stylesheet" type="text/css" media="screen, projection" href="mednafen.css" />
 </head>
 <body>
  <center><h1><img alt="Mednafen" src="mednafen.png" width="696" height="204" /></h1></center>
  <center><h1>' . htmlspecialchars($module_description) . ' Documentation</h1></center>
  <center><i>Last updated ' . strftime("%B %e, %Y", GetMTime()) . '<br />Valid as of ' . htmlspecialchars($mednafen_version) . '</i></center>';


 $toc_string = '
 <p>
 <b>Table of Contents:</b>
 <ul>';

 ob_start();
}


function EndPage()
{
 global $mednafen_version, $curmod, $cursection, $ps_string, $toc_string, $section_depth;

 $toc_string .= '</ul></p><hr width="100%" />';


 $end_string = ' </body>
</html>';


 $section_string = ob_get_contents();
 ob_end_clean();


 echo($ps_string);
 echo($toc_string);

 echo($section_string);
 echo($end_string);

 $section_depth--;
}


function BeginSection($shortname, $name)
{
 global $mednafen_version, $curmod, $cursection, $ps_string, $toc_string, $section_depth;

 $section_depth++;

 $toc_string .= '<li /><a href="#' . htmlspecialchars($shortname) . '">' . htmlspecialchars($name) . '</a><ul>';



 echo('<a name="' . htmlspecialchars($shortname) . '"><h' . $section_depth . '>' . htmlspecialchars($name) . '</h' . $section_depth . '></a><p>');
}


function EndSection()
{
 global $mednafen_version, $curmod, $cursection, $ps_string, $toc_string, $section_depth;

 $toc_string .= '</ul>';

 echo('</p><hr width="' . (100 - ($section_depth - 2) * 25) . '%">');

 $section_depth--;
}


function spis($haystack, $needle)
{
 if(strpos($haystack, $needle) === FALSE)
  return(0);
 return(1);
}

function PrintSettings()
{
 global $mednafen_version, $curmod, $cursection, $ps_string, $toc_string, $section_string, $settings;

 BeginSection('settings', 'Settings Reference');

 $value_types = array();
 $value_types['MDFNST_INT'] = "integer";
 $value_types['MDFNST_UINT'] = "integer";
 $value_types['MDFNST_BOOL'] = "boolean";
 $value_types['MDFNST_FLOAT'] = "real";
 $value_types['MDFNST_STRING'] = "string";
 $value_types['MDFNST_ENUM'] = "enum";

for($display_ct = 0; $display_ct < 2; $display_ct++)
{
 echo('<p><table border><tr class="TableHeader"><th>Setting:</th><th>Value Type:</th><th>Possible Values:</th><th>Default Value:</th><th>Description:</th></tr>');

 $row_ff = 0;
 foreach($settings as $setting)
 {
  $name = $setting['name'];
  $description = $setting['description'];
  $description_extra = $setting['description_extra'];
  $type = $setting['type'];
  $flags = $setting['flags'];

  if(strncmp($setting['name'], $curmod . '.', strlen($curmod) + 1))
   continue;

  if(strpos($flags, "MDFNSF_SUPPRESS_DOC") !== FALSE)
   continue;

  if($display_ct != spis($flags, "MDFNSF_COMMON_TEMPLATE"))
   continue;


  printf("<tr class=\"%s\"><td class=\"ColA\"><a name=\"%s\">%s%s%s</a></td><td class=\"ColB\">%s</td><td class=\"ColC\">", 
	($row_ff ? "RowB" : "RowA"), $name, (spis($flags, "MDFNSF_EMU_STATE") ? "<b>" : ""), $name,
                                        (spis($flags, "MDFNSF_EMU_STATE") ? "</b>" : ""),
                                        $value_types[$type]);



  switch($type)
  {
   case 'MDFNST_INT':
   case 'MDFNST_UINT':
   case 'MDFNST_FLOAT': printf("%s <i>through</i> %s", $setting['minimum'], $setting['maximum']);
                      break;

   case 'MDFNST_BOOL': printf("0<br>1");
                     break;

   case 'MDFNST_ENUM':
                    {
                     $enum_list = $setting['enum_list'];
		     $pending_br = '';

		     foreach($enum_list as $ele)
                     {
                      if($ele['description'] == '')
                       continue;

		      echo($pending_br);
                      printf("%s", $ele['string']);
		      $pending_br = '<br />';
                     }
                    }
                    break;
   case 'MDFNST_STRING': printf("&nbsp;");
                       break;
  }

  printf("</td><td class=\"ColD\">%s</td><td class=\"ColE\">%s<p>%s</p>", $setting['default_value'],
                                        $description, $description_extra);

  if($type == 'MDFNST_ENUM')
  {
   printf("<ul>");

   $enum_list = $setting['enum_list'];
   $pending_br = '';
   foreach($enum_list as $ele)
   {
    if($ele['description'] == '')
     continue;

    echo($pending_br);
    printf("<li><b>%s</b> - %s<br>%s</li>", $ele['string'], $ele['description'], $ele['description_extra']);

    $pending_br = '<br />';
   }

   printf("</ul>");
  }


  printf("</td></tr>");

  printf("<tr><td class=\"RowSpacer\" colspan=\"5\">&nbsp</td></tr>");

  printf("\n");



  $row_ff = !$row_ff;
 } 

 echo('</table></p>');
}


 EndSection();
}

?>
