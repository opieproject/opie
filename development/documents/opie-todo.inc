<?php

class KFeatures
{
    var $type = 'kdefeatures';
    var $parentID = null;
    var $children = array();

    function setParentID($id)
    {
        $this->parentID = $id;
    }
    function getParentID()
    {
        return $this->parentID;
    }
    function addChild(&$child)
    {
        $this->children[] = &$child;
    }
    function getHtml($target, $status)
    {
        $out = '';
        foreach ($this->children as $child) {
            $out .= $child->getHtml($target, $status);
        }
        return $out;
    }

}

class KCategory extends KFeatures
{

    var $type = 'category';
    var $name = '';
    var $children = array();

    function KCategory($name)
    {
        $this->name = $name;
    }
    function getHtml($target, $status)
    {
        $out = '';
        foreach ($this->children as $child) {
            $temp = $child->getHtml( $target, $status);
            # don't display empty categories
            if ($child->type == "category" && strlen($temp))
            {
                $out .= sprintf("\t<li>%s\t</li>\n", $temp);
            } else {
                $out .= $temp;
            }
        }
        if (strlen($out)) {
            return sprintf("<h2>%s</h2>\n\t<ul>\n%s\t</ul>\n\n",$this->name,$out);
        }
        return '';
    }
}

class KFeature extends KFeatures
{

    var $resp_name = array();
    var $resp_email = array();
    var $summary = '';
    var $status = '';
    var $target = '';
    var $type = 'feature';

    function KFeature($status, $target)
    {
        $this->status = $status;
        $this->target = $target;
    }
    function getStatus()
    {
        return $this->status;
    }
    function getTarget()
    {
        return $this->target;
    }
    function setResponsible($name = null, $email = null)
    {
        $this->resp_name[] = $name;
        $this->resp_email[] = $email;
    }
    function getResponsible()
    {
        # Nobody responsible?
        if (count($this->resp_name) == 0) return '';

        $out = '<em>';
        for ($i = 0; $i < count($this->resp_name); $i++) {
            if ($i > 0) $out .= ', ';
            if ($this->resp_name[$i]) {
                $out .= $this->resp_name[$i];
            }
            if ($this->resp_name[$i] && $this->resp_email[$i]) $out .= " ";
            if ($this->resp_email[$i]) {
                $out .= '&lt;'.$this->resp_email[$i].'&gt;';
            }
        }
        $out .= '</em>';
        return $out;
    }
    function setSummary($summary)
    {
        $this->summary .= $summary.' ';
    }
    function getHtml($target, $status){
        if ($this->target == $target && $this->status == $status) {
            return sprintf("\t\t<li>%s%s</li>\n",$this->summary,$this->getResponsible());
        }
        return '';
    }

}

function startElement($parser, $name, $attrs) {
    global $tags;
    global $parentID;

    global $insummary;
    global $pcdata;
    global $curtag;

    switch ($name) {
        case 'FEATURES':
            $parentID = 0;
            $obj = new KFeatures();
            $tags = array($obj);

            $insummary = false;
            $pcdata = '';
            $curtag = '';
            break;
        case 'CATEGORY':
            $obj = new KCategory($attrs['NAME']);
            $obj->setParentID($parentID);
            $tags[] = $obj;
            $currentID = count($tags) - 1;
            $tags[$parentID]->addChild($tags[$currentID]);
            $parentID = $currentID;

            break;
        case 'FEATURE':
            $obj = new KFeature($attrs['STATUS'], $attrs['TARGET']);
            $obj->setParentID($parentID);
            $tags[] = $obj;
            $currentID = count($tags) - 1;
            $tags[$parentID]->addChild($tags[$currentID]);
            $parentID = $currentID;
            break;
        case 'RESPONSIBLE':
            $n = count($tags) - 1;
            $tags[$n]->setResponsible(@$attrs['NAME'], @$attrs['EMAIL']);
            break;
        case 'SUMMARY':
            $insummary = true;
            $pcdata = '';
            break;
        default:
            if (!$insummary) {
                break;
            }
            $curtag = strtolower($name);
            $att = '';
            foreach ($attrs as $k => $v) {
                $att .= ' '.strtolower($k).'="'.$v.'"';
            }

            $pcdata .= '<'.$curtag.$att.'>';
            break;
    }
}

function endElement($parser, $name) {
    global $parentID;
    global $tags;

    global $curtag;
    global $pcdata;
    global $insummary;

    switch ($name) {
        case "FEATURE":
            $n = count($tags) - 1;
            $parentID = $tags[$n]->getParentID();
            break;
        case "CATEGORY":
            $parentID = $tags[$parentID]->getParentID();
            break;
        case "SUMMARY":
            $n = count($tags) - 1;
            $tags[$n]->setSummary($pcdata);

            $insummary = false;
            $pcdata = '';
            break;
        default:
            if ($insummary) {
                $pcdata .= '</'.$curtag.'>';
            }
            break;
    }
}

function characterData($parser, $data) {
    global $htmltag;
    global $tags;

    global $pcdata;
    global $curtag;
    global $insummary;

    if (!$insummary) {
        return;
    }
    $pcdata .= htmlspecialchars($data);
}

function parse( $file ) {

  $xml_parser = xml_parser_create();

  xml_parser_set_option($xml_parser, XML_OPTION_CASE_FOLDING, true);
  xml_set_element_handler($xml_parser, "startElement", "endElement");
  xml_set_character_data_handler($xml_parser, "characterData");

  if (!($fp = fopen($file, "r"))) {
      die("could not open XML input");
  }


  while ($data = fread($fp, 4096)) {
      if (!xml_parse($xml_parser, $data, feof($fp))) {
          die(sprintf("XML error: %s at line %d",
                      xml_error_string(xml_get_error_code($xml_parser)),
                      xml_get_current_line_number($xml_parser)));
      }
  }

  xml_parser_free($xml_parser);
}



function printHtml($target, $status)
{
    global $tags;
    if (!is_array($tags)) {
        die('You must parse the xml file first with parse("filename.xml");');
    }
    print($tags[0]->getHtml($target,$status));
}


?>
