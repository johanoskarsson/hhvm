<?hh

class Test implements ArrayAccess
{
    function offsetExists($offset)
    {
        echo __METHOD__ . "($offset)\n";
        return false;
    }

    function offsetGet($offset)
    {
        echo __METHOD__ . "($offset)\n";
        return null;
    }

    function offsetSet($offset, $value)
    {
        echo __METHOD__ . "($offset, $value)\n";
        throw new Exception("Ooops");
    }

    function offsetUnset($offset)
    {
        echo __METHOD__ . "($offset)\n";
    }
}
<<__EntryPoint>> function main(): void {
$list = new Test();
try
{
    $list[-1] = 123;
}
catch (Exception $e)
{
    echo "CAUGHT\n";
}

echo "===DONE===\n";
}
