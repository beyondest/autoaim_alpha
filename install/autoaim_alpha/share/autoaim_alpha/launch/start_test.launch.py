from launch import LaunchDescription
from launch_ros.actions import Node
package_name = 'autoaim_alpha'
def generate_launch_description():
    return LaunchDescription([
        Node(
            package=package_name,
            node_executable='node_test',
            node_name='node_test',
            output='screen'
        ),
        Node(
            package=package_name,
            node_executable='node_test2',
            node_name='node_test2',
            output='screen'
            
            
        ),
        Node(
            package='rviz2',
            node_executable='rviz2',
            node_name='rviz2'
        )

    ])
