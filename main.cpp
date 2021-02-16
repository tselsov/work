#include <QDebug>
#include <QRegExp>
#include <iostream>

QString getCalculateString( const QString & inputStr, bool & ok )
{ // проверка правильности введенного выражения
	if ( inputStr.contains( QRegExp( "[a-zA-Z]" ) ) || inputStr.isEmpty() )
	{
		ok = false;
		return QString();
	}
	else
	{
		QString calculateString( inputStr );
		calculateString = calculateString.replace( " ", "" );
		calculateString = calculateString.replace( ",", "." );

		ok = true;

		return calculateString;
	}
}

QStringList getExpressionList( const QString & expressionStr )
{ // преобразование строки выражения внутри скобок в список чисел и операций между ними
	QStringList resultList;
	QRegExp reg( "(-?\\d+\\.?\\d*)?\\s*(\\S)\\s*(-?\\d+\\.?\\d*)" );

	reg.indexIn( expressionStr );

	int pos = 0;
	while ( ( pos = reg.indexIn( expressionStr, pos ) ) != -1 )
	{
		auto localList = reg.capturedTexts();
		localList.removeFirst();

		resultList.append( localList );

		pos += reg.matchedLength();
	}

	resultList.removeAll( "" );

	return resultList;
}

QString getExpressionResult( const QStringList & expressionList )
{ // вычисление результата выражения внутри скобок
	QStringList list( expressionList );

	while ( list.count() > 1 )
	{
		int index = list.indexOf( "/" );
		if ( index > 0 )
		{
			if ( list.at( index + 1 ).toDouble() == 0.0 )
				return QString();
			list[ index - 1 ] = QString::number( list.at( index - 1 ).toDouble() / list.at( index + 1 ).toDouble() );
		}
		else
		{
			index = list.indexOf( "*" );
			if ( index > 0 )
				list[ index - 1 ] = QString::number( list.at( index - 1 ).toDouble() * list.at( index + 1 ).toDouble() );
			else
			{
				index = list.indexOf( "+" );
				if ( index > 0 )
					list[ index - 1 ] = QString::number( list.at( index - 1 ).toDouble() + list.at( index + 1 ).toDouble() );
				else
				{
					index = list.indexOf( "-" );
					if ( index > 0 )
						list[ index - 1 ] = QString::number( list.at( index - 1 ).toDouble() - list.at( index + 1 ).toDouble() );
					else
					{
						list[ 0 ] = QString::number( list.at( 0 ).toDouble() + list.at( 1 ).toDouble() );
						list.removeLast();

						continue;
					}
				}
			}
		}

		list.removeAt( index );
		list.removeAt( index );
	}

	return list.first();
}

double overallCalculating( const QString & inputStr )
{
	int lastOpenScobeIndex = inputStr.lastIndexOf( '(' );
	int length = inputStr.length();
	int firstCloseScobeIndex = inputStr.mid( lastOpenScobeIndex, length - lastOpenScobeIndex ).indexOf( ')' );
	firstCloseScobeIndex += lastOpenScobeIndex;

	// получаем выражение внутри скобок, не содержащие скобки
	QString expression = inputStr.mid( lastOpenScobeIndex + 1, firstCloseScobeIndex - lastOpenScobeIndex - 1 );

	// получаем результат выражения внутри скобок
	QString expressionResult = getExpressionResult( getExpressionList( expression ) );
	if ( expressionResult.isEmpty() )
	{
		printf( "Devided by zero " );
		return 0.0;
	}

	if ( lastOpenScobeIndex < 0 || firstCloseScobeIndex < 0 )
	{ // выражение не содержало скобки, выход из рекурсивного алгоритма
		return expressionResult.toDouble();
	}

	// заменяем выражение внутри скобок на его результат
	QString newExpressionStr( inputStr.mid( 0, lastOpenScobeIndex ) + expressionResult + inputStr.mid( firstCloseScobeIndex + 1, length - firstCloseScobeIndex - 1 ) );

	// рекурсиный запуск вычисления с новым выражением
	return overallCalculating( newExpressionStr );
}

int main( int argc, char * argv[] )
{
	for ( int i = 1; i < argc; i++ )
	{
		QString inputStr( argv[ i ] );
		bool correctInput = true;
		inputStr = getCalculateString( inputStr, correctInput );
		if ( correctInput )
		{
			printf( "%.2f\n", overallCalculating( inputStr ) );
		}
		else
		{
			printf( "Invalid expression\n" );
		}
	}

	return 0;
}
